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


// Block.cpp

#include <serialization/SObj.hpp>
#include <serialization/SArr.hpp>
#include "Block.hpp"
#include "../serialization/SerializationWrapper.hpp"

void Block::Serialize(std::ostream& os) const
{
	BlockHeader::Serialize(os);
	::Serialize(os, size_and_(*_txList));
}

void Block::Unserialize(std::istream& is)
{
	BlockHeader::Unserialize(is);
	if (!_txList)
	{
		_txList = std::make_shared<std::vector<std::shared_ptr<Transaction>>>();
	}
	::Unserialize(is, size_and_(*_txList));
}

SVal Block::toSVal() const
{
	SObj obj = BlockHeader::toSVal().as<SObj>();
	SArr txList;
	if (_txList)
	{
		for (auto& tx : *_txList)
		{
			txList.emplace_back(tx->toSVal());
		}
	}
	obj.emplace("transactions", std::move(txList));
	return obj;
}
