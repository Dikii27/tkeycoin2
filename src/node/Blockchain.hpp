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


// Blockchain.hpp

#pragma once


#include <configs/Setting.hpp>
#include <mutex>
#include <blockchain/Block.hpp>
#include <log/Log.hpp>
#include <utils/Timer.hpp>
#include <protocol/types/InventoryVector.hpp>
#include <protocol/types/BlockTransactions.hpp>

class Blockchain final
{
public:
	Blockchain(Blockchain&&) noexcept = delete; // Move-constructor
	Blockchain(const Blockchain&) = delete; // Copy-constructor
	Blockchain& operator=(Blockchain&&) noexcept = delete; // Move-assignment
	Blockchain& operator=(Blockchain const&) = delete; // Copy-assignment

private:
	Blockchain() = default; // Default-constructor
	~Blockchain(); // Destructor

	static Blockchain _instance;

	static Blockchain& getInstance(bool mustBeInitialized = true)
	{
		if (mustBeInitialized && !_instance._initialized)
		{
			throw std::runtime_error("Blockchain doesn't initialized yet");
		}
		return _instance;
	}

	std::unique_ptr<Log> _log;
	std::shared_ptr<Timer> _saveTimer;
	std::mutex _mutex;

	std::string _path = "mempool.dat";
	uint256 _genesisBlockHash;

	bool _initialized = false;

	std::vector<std::shared_ptr<BlockHeader>> _blocks; // blocks

	std::unordered_map<uint256, size_t> _blockIds; // block id by block hash
	std::unordered_map<size_t, const uint256&> _blockHashes; // block hash by block id
	std::unordered_multimap<uint256, size_t> _orphanBlocks; // prev block hash to current block id

	std::unordered_map<uint256, size_t> _merkles; // block id by merkle
	std::unordered_map<size_t, std::vector<size_t>> _merkleTree; // id of transactions of block

	std::vector<std::shared_ptr<Transaction>> _transactions; // transactions
	std::unordered_map<uint256, size_t> _txIds; // tx id by tx hash
	std::unordered_map<size_t, const uint256&> _txHashes; // tx hash by tx id

	std::vector<size_t> _mainChain;

	void scheduleSave();

	static void load();
	static void save();

	static std::shared_ptr<BlockHeader> getBlockHeader(size_t id);
	static std::shared_ptr<Transaction> getTx(size_t id);

	static bool connectToAncestor(size_t blockId);
	static bool connectToAncestor(const uint256& hash);

public:
	static void init(const Setting& configs);

	static size_t getHeight();

	static const uint256& getBlockHash(size_t height);

	static const uint256& getGenezisBlockHash();
	static const uint256& getTopBlockHash();

	static std::shared_ptr<BlockHeader> getTopBlockHeader();

	static std::vector<uint256> getBlockLocator(const uint256& blockHash);

	static void filterKnownInventory(std::vector<protocol::InventoryVector>& list);

	static bool hasBlockHeader(const uint256& hash);
	static bool addBlockHeader(const std::shared_ptr<BlockHeader>& header);
	static std::shared_ptr<BlockHeader> getBlockHeader(const uint256& hash);
	static std::vector<std::shared_ptr<BlockHeader>> getBlockHeaders(const std::vector<uint256>& locator, const uint256& hash);

	static bool hasBlock(const uint256& hash);
	static bool addBlock(const std::shared_ptr<Block>& block);
	static std::shared_ptr<Block> getBlock(const uint256& hash);
	static std::vector<std::shared_ptr<Block>> getBlocks(const std::vector<uint256>& locator, const uint256& hash);

	static bool hasTx(const uint256& hash);
	static bool addTx(const std::shared_ptr<Transaction>& tx);
	static bool addTxN(const protocol::BlockTransactions& transactions);
	static std::shared_ptr<Transaction> getTx(const uint256& hash);
};


