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


// Blockchain.cpp

#include <fstream>
#include <serialization/SerializationWrapper.hpp>
#include <other/HashStreams.hpp>
#include <other/MerkleTree.hpp>
#include <stack>
#include <thread/TaskManager.hpp>
#include "Blockchain.hpp"

Blockchain Blockchain::_instance;

Blockchain::~Blockchain()
{
	if (!_initialized)
	{
		return;
	}
	bool needSave = false;
	{
		std::lock_guard lockGuard(_mutex);
		needSave = _saveTimer ? _saveTimer->isActive() : false;
	}
	if (needSave)
	{
		save();
	}
}

void Blockchain::init(const Setting& setting)
{
	auto& am = getInstance(false);

	am._log = std::make_unique<Log>("blockchain");

	am._path = setting.getAs<SStr>("mempool").value();
	am._genesisBlockHash = setting.getAs<SStr>("genesis").value();

	am._initialized = true;

	load();
}

void Blockchain::scheduleSave()
{
	if (!_saveTimer)
	{
		_saveTimer = std::make_shared<Timer>(
			Blockchain::save,
			"Timeout to save mempool"
		);
	}

	_saveTimer->startOnce(std::chrono::seconds(5));
}

void Blockchain::load()
{
	auto& am = getInstance();

	// FIRST: load block headers
	// loadHeaders();

	// SECOND: load mempool
	std::vector<std::shared_ptr<BlockHeader>> blocks;
	std::vector<std::shared_ptr<Transaction>> transactions;

	{
		std::ifstream ifs(am._path, std::ios::binary);

		if (!ifs.is_open())
		{
			if (errno == ENOENT)
			{
				return;
			}
			throw std::runtime_error("Can't open mempool file '" + am._path + "' for read ← " + strerror(errno));
		}

		::Unserialize(ifs, size_and_(blocks));
		::Unserialize(ifs, size_and_(transactions));
	}

	// Fill blocks
	while (!blocks.empty())
	{
		auto&& block = blocks.back();

		size_t id = am._blocks.size();

		Hash256Stream hs;
		block->Serialize(hs);

		am._blockIds.emplace(hs.hash(), id);
		am._merkles.emplace(block->merkle(), id);
		am._blocks.emplace_back(std::forward<std::shared_ptr<BlockHeader>>(block));

		blocks.pop_back();
	}

	// Fill txs
	while (!transactions.empty())
	{
		auto&& transaction = transactions.back();

		size_t id = am._transactions.size();

		Hash256Stream hs;
		transaction->Serialize(hs);

		am._txIds.emplace(hs.hash(), id);
		am._transactions.emplace_back(std::forward<std::shared_ptr<Transaction>>(transaction));

		transactions.pop_back();
	}
}

void Blockchain::save()
{
	auto& am = getInstance();

	std::vector<std::shared_ptr<BlockHeader>> blocks;
	std::vector<std::shared_ptr<Transaction>> transactions;

	{
		std::lock_guard lockGuard(am._mutex);

		blocks.reserve(am._blocks.size());
		transactions.reserve(am._transactions.size());

		std::copy(am._blocks.begin(), am._blocks.end(), std::back_inserter(blocks));
		std::copy(am._transactions.begin(), am._transactions.end(), std::back_inserter(transactions));
	}

	auto path = am._path + "~";

	std::ofstream ofs(path, std::ios::binary | std::ios::trunc);

	if (!ofs.is_open())
	{
		throw std::runtime_error("Can't open mempool file '" + path + "' for write ← " + strerror(errno));
	}

	::Serialize(ofs, size_and_(blocks));
	::Serialize(ofs, size_and_(transactions));

	ofs.close();

	if (ofs.bad())
	{
		throw std::runtime_error("Error during writting into mempool file '" + path + "' ← " + strerror(errno));
	}

	if (rename(path.c_str(), am._path.c_str()))
	{
		throw std::runtime_error("Error at rename temporary mempool file '" + path + "' to  ← '" + am._path + "'" + strerror(errno));
	}
}

const uint256& Blockchain::getBlockHash(size_t height)
{
	auto& am = getInstance();

	if (height >= am._mainChain.size())
	{
		static const uint256 null;
		return null;
	}

	auto blockId = am._mainChain[height];

	auto i = am._blockHashes.find(blockId);
	assert(i != am._blockHashes.end());

	return i->second;
}

const uint256& Blockchain::getGenezisBlockHash()
{
	auto& am = getInstance();

	return am._genesisBlockHash;
}

size_t Blockchain::getHeight()
{
	auto& am = getInstance();

	return am._mainChain.size();
}

const uint256& Blockchain::getTopBlockHash()
{
	return getBlockHash(getHeight() - 1);
}

std::shared_ptr<BlockHeader> Blockchain::getTopBlockHeader()
{
	auto& am = getInstance();

	if (am._mainChain.empty())
	{
		return {};
	}

	return am._blocks[am._mainChain.back()];
}

bool Blockchain::hasBlockHeader(const uint256& hash)
{
	auto& am = getInstance();

	return am._blockIds.find(hash) != am._blockIds.end();
}

bool Blockchain::addBlockHeader(const std::shared_ptr<BlockHeader>& header)
{
	auto& am = getInstance();

	auto& hash = header->hash();

	if (hasBlockHeader(hash))
	{
		return false;
	}

	auto id = am._blocks.size();
	header->setId(id);
	am._blockIds.emplace(hash, id);
	am._blocks.push_back(header);
	am._blockHashes.emplace(id, hash);

	return true;
}

std::shared_ptr<BlockHeader> Blockchain::getBlockHeader(const uint256& hash)
{
	auto& am = getInstance();

	auto i = am._blockIds.find(hash);
	if (i == am._blockIds.end())
	{
		return {};
	}

	return getBlockHeader(i->second);
}

std::vector<std::shared_ptr<BlockHeader>> Blockchain::getBlockHeaders(const std::vector<uint256>& locator, const uint256& stopHash)
{
	std::vector<std::shared_ptr<BlockHeader>> headers;

	size_t height = -1;
	for (auto& hash : locator)
	{
		auto header = getBlockHeader(hash);
		if (!header)
		{
			continue;
		}
		height = header->height();
		if (height != -1)
		{
			break;
		}
	}

	for (auto& hash = getBlockHash(height); hash && hash != stopHash && headers.size() <= 1000; height++)
	{
		auto header = getBlockHeader(hash);
		headers.emplace_back(std::move(header));
	}

	return headers;
}

std::vector<std::shared_ptr<Block>> Blockchain::getBlocks(const std::vector<uint256>& locator, const uint256& stopHash)
{
	std::vector<std::shared_ptr<Block>> blocks;

	size_t height = -1;
	for (auto& hash : locator)
	{
		auto header = getBlockHeader(hash);
		if (!header)
		{
			continue;
		}
		height = header->height();
		if (height != -1)
		{
			break;
		}
	}

	for (auto& hash = getBlockHash(height); hash && hash != stopHash && blocks.size() <= 1000; height++)
	{
		auto block = getBlock(hash);
		blocks.emplace_back(std::move(block));
	}

	return blocks;
}

std::shared_ptr<BlockHeader> Blockchain::getBlockHeader(size_t id)
{
	auto& am = getInstance();

	if (id >= am._blocks.size())
	{
		return {};
	}

	return am._blocks[id];
}

std::shared_ptr<Transaction> Blockchain::getTx(size_t id)
{
	auto& am = getInstance();

	if (id >= am._transactions.size())
	{
		return {};
	}

	return am._transactions[id];
}

bool Blockchain::hasTx(const uint256& hash)
{
	auto& am = getInstance();

	return am._txIds.find(hash) != am._txIds.end();
}

bool Blockchain::addTx(const std::shared_ptr<Transaction>& tx)
{
	auto& am = getInstance();

	if (hasTx(tx->hash()))
	{
		return false;
	}

	auto id = am._transactions.size();
	am._txIds.emplace(tx->hash(), id);
	am._transactions.push_back(tx);
	am._txHashes.emplace(id, tx->hash());

	return true;
}

bool Blockchain::addTxN(const protocol::BlockTransactions& transactions)
{
	return false;
}

std::shared_ptr<Transaction> Blockchain::getTx(const uint256& hash)
{
	auto& am = getInstance();

	auto i = am._txIds.find(hash);
	if (i == am._txIds.end())
	{
		return {};
	}

	return getTx(i->second);
}


bool Blockchain::hasBlock(const uint256& hash)
{
	auto header = getBlockHeader(hash);
	return header && header->inChain();
}

bool Blockchain::addBlock(const std::shared_ptr<Block>& block)
{
	auto& am = getInstance();

	auto hash = block->hash();

	auto merkle = MerkleRoot(block->txList()->begin(), block->txList()->end());

	if (block->merkle() != merkle)
	{
		return false;
	}

	if (!hasBlockHeader(hash))
	{
		addBlockHeader(block);
	}

	for (auto& tx : *block->txList())
	{
		addTx(tx);
	}

	// connect to ancestor
	connectToAncestor(hash);

	return true;
}

bool Blockchain::connectToAncestor(size_t blockId)
{
	auto& am = getInstance();

	auto block = am.getBlockHeader(blockId);
	if (!block)
	{
		return false;
	}

	// Add genesis block
	if (getHeight() == 0)
	{
		if (block->hash() != getGenezisBlockHash() || !block->prev().isNull())
		{
			// New block is orphan
			am._orphanBlocks.emplace(block->prev(), blockId);
			return false;
		}

		block->setHeight(0);
		am._mainChain.emplace_back(blockId);
		goto descendants;
	}

	{
		// Find prev block
		size_t prevBlockId;
		{
			auto i = am._blockIds.find(block->prev());
			if (i == am._blockIds.end())
			{
				return false;
			}
			prevBlockId = i->second;
		}

		auto prevBlock = am.getBlockHeader(prevBlockId);
		if (!prevBlock)
		{
			// New block is orphan
			am._orphanBlocks.emplace(block->prev(), blockId);
			return false;
		}

		if (!prevBlock->inChain())
		{
			return false;
		}

		block->setHeight(prevBlock->height() + 1);

		// Remove from orphans list
		{
			auto range = am._orphanBlocks.equal_range(block->prev());
			for (auto i = range.first; i != range.second;)
			{
				if (i->second == blockId)
				{
					am._orphanBlocks.erase(i);
					break;
				}
			}
		}
	}

	// Check new height
	{
		auto height = am._mainChain.size() - 1;

		std::stack<std::shared_ptr<BlockHeader>> chain;

		if (block->height() >= height)
		{
			while (block && block->height() > height)
			{
				chain.push(block);
				block = getBlockHeader(block->prev());
			}

			auto newChainBlock = block;
			auto oldChainBlock = getTopBlockHeader();

			while (newChainBlock && oldChainBlock && newChainBlock != oldChainBlock)
			{
				chain.push(newChainBlock);
				am._mainChain.pop_back();

				newChainBlock = getBlockHeader(newChainBlock->prev());
				oldChainBlock = getBlockHeader(oldChainBlock->prev());
			}

			while (!chain.empty())
			{
				block = std::move(chain.top());
				chain.pop();
				assert(block->height() == am._mainChain.size());
//				block->setHeight(am._mainChain.size());
				am._mainChain.emplace_back(block->id());
			}
		}
	}

	// Find descendants
	{
		descendants:
		auto range = am._orphanBlocks.equal_range(block->hash());
		for (auto i = range.first; i != range.second;)
		{
			TaskManager::enqueue(
				[blockId = i->second] {
					Blockchain::connectToAncestor(blockId);
				}
			);
		}
	}

	return true;
}

bool Blockchain::connectToAncestor(const uint256& hash)
{
	auto& am = getInstance();

	auto i = am._blockIds.find(hash);
	assert(i != am._blockIds.end());

	return connectToAncestor(i->second);
}



//
//	auto i = am._blockIds.find(block->prev());
//	if (i == am._blockIds.end())
//
//
//		size_t blockId = -1;
//	{
//		auto i = am._blockIds.find(hash);
//		assert(i != am._blockIds.end());
//		blockId = i->second;
//	}
//
//
//	// Add genesis block
//	if (getHeight() == 0)
//	{
//		if (hash == getGenezisBlockHash() && block->prev().isNull())
//		{
//			block->setHeight(am._mainChain.size());
//			am._mainChain.emplace_back(blockId);
//			return true;
//		}
//
//	}
//	else
//	{
//		// Trace to connected ancestor
//		std::deque<size_t> chain;
//		size_t height = -1;
//
//		auto currentBlock = block;
//		size_t currBlockId = blockId;
//		for (;;)
//		{
//			auto i = am._blockIds.find(currentBlock->prev());
//			if (i == am._blockIds.end())
//			{
//				// New block is orphan
//				am._orphanBlocks.emplace(block->prev(), blockId);
//				return false;
//			}
//			size_t prevBlockId = i->second;
//
//			auto prevBlock = getBlockHeader(prevBlockId);
//			assert(prevBlock);
//
//			chain.emplace_back(currBlockId);
//
//			// prev not in chain
//			if (prevBlock->inChain())
//			{
//				height = prevBlock->height();
//				break;
//			}
//
//			currentBlock = prevBlock;
//			currBlockId = prevBlockId;
//		}
//
//		// Find descendants
//		if (!chain.empty())
//		{
//			for (;;)
//			{
//				auto lastBlockId = chain.back();
//				auto i = am._blockHashes.find(lastBlockId);
//				assert (i != am._blockHashes.end());
//				auto& lastBlockHash = i->second;
//
//				auto i2 = am._orphanBlocks.find(lastBlockHash);
//				if (i2 == am._orphanBlocks.end())
//				{
//					break;
//				}
//
//
//				chain.push_back()
//			}
//		}
//
//		// Connected ancestor not found
//		if (height == -1)
//		{
//			return false;
//		}
//
//		// Set heights
//		for (auto i = chain.rbegin(); i != chain.rend(); ++i)
//		{
//			(*i)->setHeight(++height);
//		}
//
//		// prev on top
//		if (prevBlock->height() == getHeight())
//		{
//		}
//	}
//
//	return true;
//}

std::shared_ptr<Block> Blockchain::getBlock(const uint256& hash)
{
	static const std::shared_ptr<Block> null;
	return null;  // TODO Implement getting block with his transactions (confirmed)
}

std::vector<uint256> Blockchain::getBlockLocator(const uint256& blockHash)
{
	auto& am = getInstance();

	std::vector<uint256> locator;
	locator.reserve(32);

	auto block = getBlockHeader(blockHash);

	auto height = am._mainChain.empty() ? 0 : (am._mainChain.size() - 1); // block->height();
	size_t step = 1;

	for (;;)
	{
		auto& hash = getBlockHash(height);

		locator.push_back(hash);

		if (height == 0)
		{
			break;
		}

		if (locator.size() > 10)
		{
			step <<= 1u;
		}

		height = (height > step) ? (height - step) : 0;
	}

	return locator;
}

void Blockchain::filterKnownInventory(std::vector<protocol::InventoryVector>& inventory)
{
	inventory.erase(
		std::remove_if(inventory.begin(), inventory.end(),
			[]
			(auto& item)
			{
				switch (item.type())
				{
					case protocol::InventoryVector::Type::MSG_TX:
					{
						bool exists = hasTx(item.hash());
						return exists;
					}
					case protocol::InventoryVector::Type::MSG_BLOCK:
					case protocol::InventoryVector::Type::MSG_FILTERED_BLOCK:
					case protocol::InventoryVector::Type::MSG_CMPCT_BLOCK:
					{
						bool exists = hasBlock(item.hash());
						return exists;
					}

					default:
						return true;
				}
			}
		),
		inventory.end()
	);
}
