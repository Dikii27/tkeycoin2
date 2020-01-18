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


// BlockTransactionsRequest.hpp

#pragma once


#include <types/Blobs.hpp>

namespace protocol
{

class BlockTransactionsRequest final : public Serializable
{
	uint256 _hash;                  // The blockhash of the block which the transactions being requested are in
	std::vector<uintV> _indexes;    // The indexes of the transactions being requested in the block

public:
	BlockTransactionsRequest() = default; // Default-constructor
	BlockTransactionsRequest(BlockTransactionsRequest&&) noexcept = delete; // Move-constructor
	BlockTransactionsRequest(const BlockTransactionsRequest&) = default; // Copy-constructor
	~BlockTransactionsRequest() = default; // Destructor
	BlockTransactionsRequest& operator=(BlockTransactionsRequest&&) noexcept = delete; // Move-assignment
	BlockTransactionsRequest& operator=(BlockTransactionsRequest const&) = delete; // Copy-assignment

	BlockTransactionsRequest(
		uint256 blockhash,
		std::vector<uintV> indexes
	)
	: _hash(std::move(blockhash))
	, _indexes(std::move(indexes))
	{}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;
};

}
