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


// BlockTransactionsRequest.cpp

#include <types/VariableLengthInteger.hpp>
#include <serialization/SObj.hpp>
#include <serialization/SArr.hpp>
#include "BlockTransactionsRequest.hpp"

void protocol::BlockTransactionsRequest::Serialize(std::ostream& os) const
{
	_hash.Serialize(os);

	uintV indexCount = _indexes.size();
	indexCount.Serialize(os);

	uint32_t prev = -1;
	for (auto index : _indexes)
	{
		if (prev == -1)
		{
			index.Serialize(os);
			prev = index;
			continue;
		}
		::Serialize(os, index - prev - 1);
		prev = index;
	}
}

void protocol::BlockTransactionsRequest::Unserialize(std::istream& is)
{
	::Unserialize(is, _hash);

	uintV indexCount;
	::Unserialize(is, indexCount);

	_indexes.resize(indexCount);
	::Unserialize(is, _indexes);

	uint32_t prev = -1;
	for (auto& index : _indexes)
	{
		if (prev == -1)
		{
			prev = index;
			continue;
		}
		index = index + prev + 1;
		prev = index;
	}
}

SVal protocol::BlockTransactionsRequest::toSVal() const
{
	SObj obj;
	obj.emplace("hash", _hash.toSVal());
	SArr arr;
	for (auto& item : _indexes)
	{
		arr.emplace_back(item);
	}
	obj.emplace("indexes", std::move(arr));
	return obj;
}
