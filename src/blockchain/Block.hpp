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
//


// Block.hpp

#pragma once

#include "BlockHeader.hpp"
#include "Transaction.hpp"

class Block : public BlockHeader
{
private:
	BlockHeader& _header = static_cast<BlockHeader&>(*this);

	std::shared_ptr<std::vector<std::shared_ptr<Transaction>>> _txList;

public:
	Block() = default; // Default-constructor
	Block(Block&&) noexcept = delete; // Move-constructor
	Block(const Block&) = delete; // Copy-constructor
	~Block() = default; // Destructor
	Block& operator=(Block&&) noexcept = delete; // Move-assignment
	Block& operator=(Block const&) = delete; // Copy-assignment

	explicit Block(const BlockHeader& header)
	: BlockHeader(header)
	{
	}

	explicit Block(BlockHeader&& header)
	: BlockHeader(header)
	{
	}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;

	auto& txList()
	{
		return _txList;
	}
};


