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


// TxOutPoint.hpp

#pragma once

#include "../serialization/Serialization.hpp"
#include "../types/Blobs.hpp"

class TxOutPoint : public Serializable
{
private:
	uint256 _hash;  //	The hash of the referenced transaction.
	uint32_t _index; // The index of the specific output in the transaction. The first output is 0, etc.

public:
	TxOutPoint(TxOutPoint&&) noexcept = default; // Move-constructor
	TxOutPoint(const TxOutPoint&) = default; // Copy-constructor
	~TxOutPoint() = default; // Destructor
	TxOutPoint& operator=(TxOutPoint&&) noexcept = delete; // Move-assignment
	TxOutPoint& operator=(TxOutPoint const&) = delete; // Copy-assignment

	TxOutPoint() // Default-constructor
	: _index(std::numeric_limits<decltype(_index)>::max())
	{
	}

	TxOutPoint(uint256&& hash, uint32_t index)
	: _hash(std::forward<uint256>(hash))
	, _index(index)
	{
	}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;
};


