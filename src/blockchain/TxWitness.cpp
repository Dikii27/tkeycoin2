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


// TxWitness.cpp

#include <serialization/SArr.hpp>
#include "TxWitness.hpp"
#include "../serialization/SerializationWrapper.hpp"

void TxWitness::Serialize(std::ostream& os) const
{
	::Serialize(os, size_and_(_stack));
}

void TxWitness::Unserialize(std::istream& is)
{
	::Unserialize(is, size_and_(_stack));
}

SVal TxWitness::toSVal() const
{
	SArr arr;
	for (auto& item : _stack)
	{
		SArr arr2;
		for (auto& item2 : item)
		{
			arr2.emplace_back(item2);
		}
		arr.emplace_back(std::move(arr2));
	}
	return arr;
}
