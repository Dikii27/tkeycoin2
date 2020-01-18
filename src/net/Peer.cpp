//  Copyright (c) 2017-2020 TKEY DMCC LLC & Tkeycoin Dao. All rights reserved.
//  Website: www.tkeycoin.com
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.


// Peer.cpp

#include <utils/Daemon.hpp>
#include <transport/messages/OutputConnectionStream.hpp>
#include <protocol/messages/Verack.hpp>
#include <transport/messages/MsgContext.hpp>
#include <cassert>
#include <protocol/messages/SendHeaders.hpp>
#include <protocol/messages/Ping.hpp>
#include <protocol/messages/Pong.hpp>
#include <protocol/messages/SendCmpct.hpp>
#include <protocol/messages/GetHeaders.hpp>
#include <protocol/messages/FeeFilter.hpp>
#include <protocol/messages/GetAddr.hpp>
#include <protocol/messages/Addr.hpp>
#include <protocol/messages/GetBlocks.hpp>
#include <protocol/messages/GetBlockTxn.hpp>
#include <protocol/messages/GetData.hpp>
#include <protocol/messages/Tx.hpp>
#include <protocol/messages/Block.hpp>
#include <protocol/messages/CmpctBlock.hpp>
#include <protocol/messages/NotFound.hpp>
#include <protocol/messages/Inv.hpp>
#include <node/AddressManager.hpp>
#include <protocol/messages/Headers.hpp>
#include <thread/TaskManager.hpp>
#include "Peer.hpp"
#include "PeerManager.hpp"

std::atomic_uint64_t Peer::_lastId(0);

Peer::Peer()
: _id(_lastId.fetch_add(1, std::memory_order_relaxed))
, _pingNonce(GetRand())
{
}

void Peer::touch(bool temporary)
{
	if (!_timeoutForUnload)
	{
		_timeoutForUnload = std::make_shared<Timer>(
			[wp = std::weak_ptr(ptr())]
			{
				auto peer = wp.lock();
				if (peer)
				{
					peer->close(Daemon::shutingdown() ? "shuting down" : "timeout");
				}
			},
			"Timeout to close peer"
		);
	}

	_timeoutForUnload->prolong(delayBeforeUnload(temporary));
}

void Peer::alive()
{
	if (!_pingTimer)
	{
		_pingTimer = std::make_shared<Timer>(
			[wp = std::weak_ptr(ptr())] {
				auto peer = wp.lock();
				if (peer)
				{
					if (peer->_pingNonce)
					{
						peer->close(Daemon::shutingdown() ? "shuting down" : "Pong timeout");
					}
					else
					{
						peer->_pingNonce = GetRand();
						peer->_context->transmit(protocol::message::Ping(peer->_pingNonce));
					}
					peer->_pingTimer->prolong(peer->pingInterval());
				}
			},
			"Timeout to ping-pong"
		);
	}

	_pingNonce = 0;
	_pingTimer->prolong(pingInterval());
}

void Peer::close(const std::string& reason)
{
	PeerManager::closePeer(ptr());
}

void Peer::initialSetup(const std::shared_ptr<Node>& node) const
{
	protocol::message::SendHeaders msgSendHeaders;
	_context->transmit(msgSendHeaders);

	protocol::message::SendCmpct msgSendCmpct(false, 1);
	_context->transmit(msgSendCmpct);

	protocol::message::Ping msgPing(_pingNonce);
	_context->transmit(msgPing);

	if (Blockchain::getHeight())
	{
		AskHeaders(
			node,
			Blockchain::getBlockLocator(Blockchain::getTopBlockHash()),
			Blockchain::getTopBlockHash()
		);
	}
	else
	{
		protocol::message::GetData getData({{
			protocol::InventoryVector::Type::MSG_BLOCK,
			Blockchain::getGenezisBlockHash()
		}});
		_context->transmit(getData);
	}

	protocol::message::FeeFilter msgFeeFilter(1000); // TODO set really fee rate
	_context->transmit(msgFeeFilter);

	protocol::message::GetAddr msgGetAddr; // TODO To use if realy needed
	_context->transmit(msgGetAddr);
}

void Peer::ping(uint64_t nonce) const
{
	protocol::message::Pong msgPong(nonce);
	_context->transmit(msgPong);
}

void Peer::pong(uint64_t nonce)
{
	if (nonce != _pingNonce)
	{
		close("Wrong Ping-Pong nonce");
		return;
	}
	alive();
}

void Peer::AskHeaders(
	const std::shared_ptr<Node>& node,
	const std::vector<uint256>& locatorHashes,
	const uint256& stopHash
) const
{
	protocol::message::GetHeaders msgGetHeaders(
		node->VERSION,
		locatorHashes,
		stopHash
	);
	_context->transmit(msgGetHeaders);
}

void Peer::ReceiveHeaders(
	const std::shared_ptr<Node>& node,
	const std::vector<std::shared_ptr<BlockHeader>>& headers
)
{
	std::vector<protocol::InventoryVector> inventory;
	for (auto& header : headers)
	{
		if (Blockchain::addBlockHeader(header))
		{
			inventory.emplace_back(protocol::InventoryVector::Type::MSG_BLOCK, header->hash());

//			protocol::message::GetBlockTxn msgGetBlockTxn(header->hash(), std::move(headers));
//			_context->transmit(msgGetBlockTxn);
//			protocol::message::GetBlockTxn msgGetBlocks(header->hash(), std::move(headers));
//			_context->transmit(msgGetBlockTxn);
		}
	}
	if (!inventory.empty())
	{
//inventory.resize(3);
		AskInventory(node, inventory);

		TaskManager::enqueue(
			[
				peerWp = std::weak_ptr(ptr()),
				nodeWp = std::weak_ptr(node)
			]
			{
				auto peer = peerWp.lock();
				auto node = nodeWp.lock();
				if (!peer || !node)
				{
					return;
				}
				peer->AskHeaders(
					node,
					Blockchain::getBlockLocator(Blockchain::getTopBlockHash()),
					Blockchain::getTopBlockHash()
				);
			},
			std::chrono::seconds(5),
			"To ask next portion of headers"
		);
	}
}

void Peer::SendHeaders(
	const std::shared_ptr<Node>& node,
	const std::vector<uint256>& locatorHashes,
	const uint256& stopHash
) const
{
	auto headers = Blockchain::getBlockHeaders(locatorHashes, stopHash);
	if (headers.empty())
	{
		return;
	}
	protocol::message::Headers msgHeaders(std::move(headers));
	_context->transmit(msgHeaders);
}

void Peer::AskBlocks(
	const std::shared_ptr<Node>& node,
	const std::vector<uint256>& locatorHashes,
	const uint256& stopHash
) const
{
	protocol::message::GetBlocks msgGetBlocks(
		node->VERSION,
		locatorHashes,
		stopHash
	);
	_context->transmit(msgGetBlocks);
}

void Peer::ReceiveBlock(const std::shared_ptr<Node>& node, const std::shared_ptr<const Block>& block) const
{
	// TODO To implement receiving of block headers
}

void Peer::SendBlocks(
	const std::shared_ptr<Node>& node,
	const std::vector<uint256>& locatorHashes,
	const uint256& stopHash
) const
{
	auto blocks = Blockchain::getBlocks(locatorHashes, stopHash);
	for (auto& block : blocks)
	{
		protocol::message::Block msgBlock(std::move(block));
		_context->transmit(msgBlock);
	}
}

void Peer::AskAddress(const std::shared_ptr<Node>& node) const
{
	protocol::message::GetAddr msgGetAddr;
	_context->transmit(msgGetAddr);
}

void Peer::ReceiveAddress(
	const std::shared_ptr<Node>& node,
	const std::vector<protocol::NetworkAddress>& addresses
) const
{
	for (auto& address : addresses)
	{
		AddressManager::reg(address);
	}
}

void Peer::SendAddress(const std::shared_ptr<Node>& node) const
{
	protocol::message::Addr msgAddr(AddressManager::get(1000));
	_context->transmit(msgAddr);
}

void Peer::AskInventory(
	const std::shared_ptr<Node>& node,
	const std::vector<protocol::InventoryVector>& inventory_
) const
{
	auto inventory = inventory_;

	if (inventory.empty())
	{
		return;
	}

	Blockchain::filterKnownInventory(inventory);

	if (!inventory.empty())
	{
		protocol::message::GetData msgGetData(std::move(inventory));
		_context->transmit(msgGetData);
	}
}

void Peer::ReceiveInventory(
	const std::shared_ptr<Node>& node,
	const std::vector<protocol::InventoryVector>& inventory
) const
{
//	auto inventory = inventory_;

	std::vector<protocol::InventoryVector> notFoundInventory;

	for (auto& item : inventory)
	{
		switch (item.type())
		{
			case protocol::InventoryVector::Type::MSG_TX:
				if (auto tx = Blockchain::getTx(item.hash()))
				{
					protocol::message::Tx msgTx(tx);
					_context->transmit(msgTx);
				}
				else
				{
					notFoundInventory.emplace_back(item);
				}
				break;

			case protocol::InventoryVector::Type::MSG_BLOCK:
				if (auto block = Blockchain::getBlock(item.hash()))
				{
					protocol::message::Block msgBlock(block);
					_context->transmit(msgBlock);
				}
				else
				{
					notFoundInventory.emplace_back(item);
				}
				break;

//			case protocol::InventoryVector::Type::MSG_FILTERED_BLOCK:
//				if (auto& block = node->getBlock(item.hash()))
//				{
//					protocol::message::Block msgBlock(block);
//					_context->transmit(msgBlock);
//				}
//				else
//				{
//					notFoundInventory.emplace_back(item);
//				}
//				break;
//
//			case protocol::InventoryVector::Type::MSG_CMPCT_BLOCK:
//				if (auto& block = node->getBlock(item.hash()))
//				{
//					protocol::message::CmpctBlock msgCmpctBlock(block);
//					_context->transmit(msgCmpctBlock);
//				}
//				else
//				{
//					notFoundInventory.emplace_back(item);
//				}
//				break;

			default:;
		}
	}

	if (!notFoundInventory.empty())
	{
		protocol::message::NotFound msgNotFound(std::move(notFoundInventory));
		_context->transmit(msgNotFound);
	}
}

void Peer::SendInventory(const protocol::InventoryVector& item)
{
	std::lock_guard lockGuard(_inventoryAnnounceMutex);

	if (!_inventoryAnnounceTimer)
	{
		_inventoryAnnounceTimer = std::make_shared<Timer>(
			[wp = std::weak_ptr(ptr())]
			{
				auto peer = wp.lock();
				if (!peer) return;

				std::lock_guard lockGuard(peer->_inventoryAnnounceMutex);

				std::vector<protocol::InventoryVector> inventory;
				inventory.reserve(std::min<size_t>(Node::MAX_INV_COUNT, peer->_inventoryForAnnounce.size()));

				size_t count = 0;
				for (auto& item : peer->_inventoryForAnnounce)
				{
					inventory.emplace_back(item);

					++count;
					if (count == Node::MAX_INV_COUNT || count == peer->_inventoryForAnnounce.size())
					{
						protocol::message::Inv msgInv(std::move(inventory));
						peer->getContext()->transmit(msgInv);
					}
				}

				peer->_inventoryForAnnounce.clear();
			},
			"Timeout to send inv message"
		);
	}

	_inventoryForAnnounce.emplace(item);
	_inventoryAnnounceTimer->startOnce(std::chrono::seconds(5));
}

void Peer::AskBlockTxN(
	const std::shared_ptr<Node>& node,
	const protocol::BlockTransactionsRequest& request
) const
{
}

void Peer::ReceiveBlockTxN(
	const std::shared_ptr<Node>& node,
	const std::shared_ptr<const Block>& block
) const
{
}

void Peer::SendBlockTxB(
	const std::shared_ptr<Node>& node,
	const std::vector<uint256>& locatorHashes,
	const uint256& stopHash
) const
{
}
