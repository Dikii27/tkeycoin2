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


// PrecomputedTransactionData.cpp

#include <blockchain/Transaction.hpp>
#include <other/hash.h>
#include <other/HashWriter.hpp>
#include "PrecomputedTransactionData.hpp"
#include <serialization/SerializationActionType.hpp>

template<class T>
uint256 GetPrevoutHash(const T& tx)
{
	CHashWriter ss(SerializationActionType::SER_GETHASH, 0);
	for (const auto& txIn : tx.txIns())
	{
		ss << txIn.prevOut();
	}
	return ss.GetHash();
}

template<class T>
uint256 GetSequenceHash(const T& tx)
{
	CHashWriter ss(SerializationActionType::SER_GETHASH, 0);
	for (const auto& txIn : tx.txIns())
	{
		ss << txIn.sequence();
	}
	return ss.GetHash();
}

template<class T>
uint256 GetOutputsHash(const T& tx)
{
	CHashWriter ss(SerializationActionType::SER_GETHASH, 0);
	for (const auto& txOut : tx.txOuts())
	{
		ss << txOut;
	}
	return ss.GetHash();
}

template<class T>
PrecomputedTransactionData::PrecomputedTransactionData(const T& tx)
{
	// Cache is calculated only for transactions with witness
	if (tx.HasWitness())
	{
		hashPrevouts = GetPrevoutHash(tx);
		hashSequence = GetSequenceHash(tx);
		hashOutputs = GetOutputsHash(tx);
		ready = true;
	}
}

// explicit instantiation
template PrecomputedTransactionData::PrecomputedTransactionData(const Transaction& tx);
