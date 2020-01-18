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


// Transaction.cpp

#include <serialization/SObj.hpp>
#include <serialization/SArr.hpp>
#include <other/HashStreams.hpp>
#include "Transaction.hpp"
#include "../serialization/SerializationWrapper.hpp"

void Transaction::Serialize(std::ostream& os) const
{
	::Serialize(os, _version);
	if (_flag)
	{
		::Serialize(os, *_flag);
	}
	::SerializeList(os,
		size_and_(_txIn),
		size_and_(_txOut)
	);
	if (_flag)
	{
		for (auto& txIn : _txIn)
		{
			::Serialize(os, txIn.scriptWitness());
		}
	}
	::Serialize(os, _srcChain);
	::Serialize(os, _dstChain);
	::Serialize(os, _lockTime);
}

void Transaction::Unserialize(std::istream& is)
{
	::Unserialize(is, _version);
	_flag.reset();
	if (is.peek() == 0)
	{
		uint16_t flag{};
		::Unserialize(is, flag);
		_flag.emplace(flag);
	}
	::UnserializeList(is,
		size_and_(_txIn),
		size_and_(_txOut)
	);
	if (_flag)
	{
		for (auto& txIn : _txIn)
		{
			::Unserialize(is, txIn.scriptWitness());
		}
	}
	::Unserialize(is, _srcChain);
	::Unserialize(is, _dstChain);
	::Unserialize(is, _lockTime);
}

SVal Transaction::toSVal() const
{
	SObj obj;
	obj.emplace("version", _version);
	if (_flag)
	{
		obj.emplace("flag", *_flag);
	}
	SArr txIn;
	for (auto& item : _txIn)
	{
		txIn.emplace_back(item.toSVal());
	}
	obj.emplace("in", std::move(txIn));
	SArr txOut;
	for (auto& item : _txOut)
	{
		txOut.emplace_back(item.toSVal());
	}
	obj.emplace("srcChain", _srcChain);
	obj.emplace("dstChain", _dstChain);
	obj.emplace("out", std::move(txOut));
	return obj;
}

const uint256& Transaction::hash() const
{
	if (!_validHash)
	{
		Hash256Stream hs;
		Transaction::Serialize(hs);
		_hash = hs.hash();
		_validHash = true;
	}
	return _hash;
}
