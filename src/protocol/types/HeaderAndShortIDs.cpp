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


// HeaderAndShortIDs.cpp

#include <serialization/SerializationWrapper.hpp>
#include <serialization/SObj.hpp>
#include <serialization/SArr.hpp>
#include "HeaderAndShortIDs.hpp"

void protocol::HeaderAndShortIDs::Serialize(std::ostream& os) const
{
	SerializeList(
		os,
		_header,
		_nonce,
		size_and_(_shortIds),
		size_and_(_prefilledTxn)
	);
}

void protocol::HeaderAndShortIDs::Unserialize(std::istream& is)
{
	UnserializeList(
		is,
		_header,
		_nonce,
		size_and_(_shortIds),
		size_and_(_prefilledTxn)
	);
}

SVal protocol::HeaderAndShortIDs::toSVal() const
{
	SObj obj;
	obj.emplace("header", _header->toSVal());
	obj.emplace("nonce", _nonce);
	SArr arr;
	for (auto& shortId : _shortIds)
	{
		arr.emplace_back(shortId.toSVal());
	}
	obj.emplace("shortIds", std::move(arr));
	for (auto& prefilledTxn : _prefilledTxn)
	{
		arr.emplace_back(prefilledTxn.toSVal());
	}
	obj.emplace("shortIds", std::move(arr));
	return obj;
}
