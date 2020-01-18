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


// Node.cpp

#include <transport/Transports.hpp>
#include <transport/messages/MsgCommunicator.hpp>
#include <thread/Thread.hpp>
#include <net/PeerManager.hpp>
#include <transport/messages/MsgContext.hpp>
#include <cassert>
#include <protocol/messages/Tx.hpp>
#include <protocol/messages/Block.hpp>
#include <other/HashStreams.hpp>
#include "Node.hpp"
#include "Blockchain.hpp"

Node::Node(const Setting& configs)
{
	try
	{
//		auto& nodeConfig = configs.getAs<SObj>("node");
//
//		// Init blockchain
//		{
//			auto& blockchainName = nodeConfig.getAs<SStr>("blockchain");
//
//			auto& blockchainConfig = configs.getAs<SObj>("blockchain").getAs<SObj>(blockchainName);
//
//			_blockchain = std::make_shared<Blockchain>(blockchainName, blockchainConfig);
//		}
	}
	catch (const std::exception& exception)
	{
		throw std::runtime_error(std::string() + "Fail configure node ← " + exception.what());
	}
}

void Node::up()
{
	// Up protocol
	if (auto transport = Transports::get("protocol"))
	{
		if (!transport->enable())
		{
			throw std::runtime_error("Can't enable transport 'protocol'");
		}
		_protocol = std::make_shared<Protocol>(ptr());
		try
		{
			transport->bindHandler(
				"/",
				std::make_shared<ServerTransport::Handler>(
					[wp = std::weak_ptr(_protocol)]
					(const std::shared_ptr<Context>& context)
					{
						auto iam = wp.lock();
						if (iam)
						{
							iam->handler(context);
						}
					}
				)
			);
		}
		catch (const std::exception& exception)
		{
			throw std::runtime_error(std::string() + "Can't bind uri '/' on transport 'protocol': " + exception.what());
		}
	}

	// Up RPC
	if (auto transport = Transports::get("rpc"))
	{
		if (!transport->enable())
		{
			throw std::runtime_error("Can't enable transport 'rpc'");
		}
		_rpc = std::make_shared<RPC>(ptr());
		try
		{
			transport->bindHandler(
				"/",
				std::make_shared<ServerTransport::Handler>(
					[wp = std::weak_ptr(_rpc)]
						(const std::shared_ptr<Context>& context) {
						auto iam = wp.lock();
						if (iam)
						{
							iam->handler(context);
						}
					}
				)
			);
		}
		catch (const std::exception& exception)
		{
			throw std::runtime_error(std::string() + "Can't bind uri '/' on transport 'rpc': " + exception.what());
		}
	}

	connectToPeers();
}

void Node::down()
{
	// Down protocol
	if (auto transport = Transports::get("protocol"))
	{
		transport->disable();
	}
	_protocol.reset();

	// Down RPC
	if (auto transport = Transports::get("rpc"))
	{
		transport->disable();
	}
	_rpc.reset();
}

void Node::connectToPeers()
{
	auto onSuccessHandler = std::make_shared<Transport::Handler>(
		[wp = std::weak_ptr(ptr())]
		(const std::shared_ptr<Context>& context_)
		{
			auto context = std::dynamic_pointer_cast<MsgContext>(context_);
			assert(context);

			auto node = wp.lock();
			if (!node)
			{
				return;
			}

			auto peer = PeerManager::newPeer();
			peer->protectedDo([&]{
				peer->assignContext(context);
				context->assignPeer(peer);
			});

			peer->initialSetup(node);

			context->setHandler(std::make_shared<Transport::Handler>(
				[wp]
				(const std::shared_ptr<Context>& context)
				{
					if (auto node = wp.lock())
					{
						node->protocol()->handler(context);
					}
				}
			));
		}
	);

	auto onFailHandler = std::make_shared<std::function<void(MsgCommunicator&)>>(
		[wp = std::weak_ptr(ptr())]
		(MsgCommunicator& communicator)
		{
			if (auto iam = wp.lock())
			{
				iam->_peers.erase(communicator.ptr());
			}
		}
	);

	for (auto i = 0; i < 1; ++i)
	{
		auto msgcomm = std::make_shared<MsgCommunicator>(
			ptr(),
			HttpUri("tcp://127.0.0.1:20445"),
			onSuccessHandler,
			onFailHandler
		);

//		auto msgcomm = std::make_shared<MsgCommunicator>(
//			HttpUri("tcp://node4.tkeycoin.com:20445"),
//			onSuccessHandler,
//			onFailHandler
//		);

//		auto msgcomm = std::make_shared<MsgCommunicator>(HttpUri("tcp://ya.ru:80"), onSuccessHandler, onFailHandler);

		_peers.emplace(msgcomm);

		(*msgcomm)();

		msgcomm->hasFailed();
	}
}

void Node::announceTx(const std::shared_ptr<Transaction>& tx) const
{
	Hash256Stream s;
	tx->Serialize(s);
	protocol::InventoryVector item(protocol::InventoryVector::Type::MSG_TX, s.hash());

	PeerManager::forEach(
		[&item]
		(const std::shared_ptr<Peer>& peer)
		{
			peer->SendInventory(item);
		}
	);
}

void Node::announceBlock(const std::shared_ptr<Block>& block) const
{
	Hash256Stream s;
	block->Serialize(s);
	protocol::InventoryVector item(protocol::InventoryVector::Type::MSG_BLOCK, s.hash());

	PeerManager::forEach(
		[&item]
		(const std::shared_ptr<Peer>& peer)
		{
			peer->SendInventory(item);
		}
	);
}
