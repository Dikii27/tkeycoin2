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


// Peer.hpp

#pragma once


#include <sessions/Communicate.hpp>
#include <atomic>
#include <types/Blobs.hpp>
#include <protocol/types/NetworkAddress.hpp>
#include <protocol/types/InventoryVector.hpp>
#include <unordered_set>
#include <blockchain/BlockHeader.hpp>
#include <blockchain/Block.hpp>
#include <protocol/types/BlockTransactionsRequest.hpp>

class Node;
class MsgContext;

class Peer final : public Shareable<Peer>
{
public:
	typedef uint64_t Id;

private:
	static std::atomic_uint64_t _lastId;

	mutable std::recursive_mutex _mutex;

	std::chrono::seconds pingInterval() const
	{
		return std::chrono::seconds(300);
	}

	void alive();

	std::shared_ptr<Timer> _pingTimer;


	virtual std::chrono::seconds delayBeforeUnload(bool isShort) const
	{
		return isShort ? std::chrono::seconds(15) : std::chrono::seconds(900);
	}

	std::shared_ptr<MsgContext> _context;

	/// Таймаут выгрузки
	std::shared_ptr<Timer> _timeoutForUnload;

	Id _id;
	int32_t _version = 0;
	bool _sendHeaders = false;
	uint64_t _compactVersion = 0;
	uint64_t _pingNonce = 0;
	uint64_t _feeRate = 0;

	std::mutex _inventoryAnnounceMutex;
	std::shared_ptr<Timer> _inventoryAnnounceTimer;
	std::unordered_set<protocol::InventoryVector> _inventoryForAnnounce;

	friend class PeerManager;

	Peer(); // Default-constructor
	~Peer() override = default; // Destructor

public:
	Peer(Peer&&) noexcept = delete; // Move-constructor
	Peer(const Peer&) = delete; // Copy-constructor
	Peer& operator=(Peer&&) noexcept = delete; // Move-assignment
	Peer& operator=(Peer const&) = delete; // Copy-assignment

	void touch(bool temporary = false);

	void close(const std::string& reason);

	void assignContext(const std::shared_ptr<MsgContext>& context)
	{
		std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
		_context = context;
	}
	std::shared_ptr<MsgContext> getContext()
	{
		return _context;
	}

	void protectedDo(const std::function<void()>& func)
	{
		std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
		func();
	}

	Id id() const
	{
		return _id;
	}

	void initialSetup(const std::shared_ptr<Node>& node) const;

	int32_t version() const
	{
		return _version;
	}

	void setVersion(int32_t version)
	{
		_version = version;
	}

	void setSendHeaders()
	{
		_sendHeaders = true;
	}

	void setCompact(bool announce, uint64_t version)
	{
		_compactVersion = announce ? version : 0;
	}

	void setFeeFilter(uint64_t feeRate)
	{
		_feeRate = feeRate;
	}

	void AskHeaders(
		const std::shared_ptr<Node>& node,
		const std::vector<uint256>& locatorHashes,
		const uint256& stopHash
	) const;
	void ReceiveHeaders(
		const std::shared_ptr<Node>& node,
		const std::vector<std::shared_ptr<BlockHeader>>& headers
	);
	void SendHeaders(
		const std::shared_ptr<Node>& node,
		const std::vector<uint256>& locatorHashes,
		const uint256& stopHash
	) const;

	void AskBlocks(
		const std::shared_ptr<Node>& node,
		const std::vector<uint256>& locatorHashes,
		const uint256& stopHash
	) const;
	void ReceiveBlock(const std::shared_ptr<Node>& node, const std::shared_ptr<const Block>& block) const;
	void SendBlocks(
		const std::shared_ptr<Node>& node,
		const std::vector<uint256>& locatorHashes,
		const uint256& stopHash
	) const;

	void AskBlockTxN(
		const std::shared_ptr<Node>& node,
		const protocol::BlockTransactionsRequest& request
	) const;
	void ReceiveBlockTxN(const std::shared_ptr<Node>& node, const std::shared_ptr<const Block>& block) const;
	void SendBlockTxB(
		const std::shared_ptr<Node>& node,
		const std::vector<uint256>& locatorHashes,
		const uint256& stopHash
	) const;

	void AskAddress(const std::shared_ptr<Node>& node) const;
	void ReceiveAddress(
		const std::shared_ptr<Node>& node,
		const std::vector<protocol::NetworkAddress>& addresses
	) const;
	void SendAddress(const std::shared_ptr<Node>& node) const;

	void AskInventory(
		const std::shared_ptr<Node>& node,
		const std::vector<protocol::InventoryVector>& inventory
	) const;
	void ReceiveInventory(
		const std::shared_ptr<Node>& node,
		const std::vector<protocol::InventoryVector>& inventory
	) const;
	void SendInventory(const protocol::InventoryVector& item);

	void ping(uint64_t nonce) const;
	void pong(uint64_t nonce);
};
