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


// SignatureHash.cpp

#include <other/HashWriter.hpp>
#include <blockchain/Transaction.hpp>
#include "SignatureHash.hpp"
#include "SignatureHashType.hpp"
#include "TransactionSignatureSerializer.hpp"

template<class T>
uint256 SignatureHash(
	const Script& scriptCode,
	const T& tx,
	unsigned int nIn,
	Flags<SignatureHashType> nHashType,
	const Amount& amount,
	SigVersion sigversion,
	const PrecomputedTransactionData* cache
)
{
	assert(nIn < tx.txIns().size());

	auto maskedHashType = nHashType.mask(0x1f);

//	if (sigversion == SigVersion::WITNESS_V0)
//	{
//		uint256 hashPrevouts;
//		uint256 hashSequence;
//		uint256 hashOutputs;
//		const bool cacheready = cache && cache->ready;
//
//		if (!nHashType.isSet(SignatureHashType::SIGHASH_ANYONECANPAY))
//		{
//			hashPrevouts = cacheready ? cache->hashPrevouts : GetPrevoutHash(tx);
//		}
//
//		if (
//			!nHashType.isSet(SignatureHashType::SIGHASH_ANYONECANPAY) &&
//			!maskedHashType.isSet(SignatureHashType::SIGHASH_SINGLE) &&
//			!maskedHashType.isSet(SignatureHashType::SIGHASH_NONE)
//		)
//		{
//			hashSequence = cacheready ? cache->hashSequence : GetSequenceHash(tx);
//		}
//
//
//		if (
//			!maskedHashType.isSet(SignatureHashType::SIGHASH_SINGLE) &&
//			!maskedHashType.isSet(SignatureHashType::SIGHASH_NONE)
//		)
//		{
//			hashOutputs = cacheready ? cache->hashOutputs : GetOutputsHash(tx);
//		}
//		else if (
//			maskedHashType.isSet(SignatureHashType::SIGHASH_SINGLE) &&
//			nIn < tx.txOuts().size()
//		)
//		{
//			CHashWriter ss(SerializationActionType::SER_GETHASH, 0);
//			ss << tx.txOut(nIn);
//			hashOutputs = ss.GetHash();
//		}
//
//		CHashWriter ss(SerializationActionType::SER_GETHASH, 0);
//		// Version
//		ss << tx.nVersion;
//		// Input prevouts/nSequence (none/all, depending on flags)
//		ss << hashPrevouts;
//		ss << hashSequence;
//		// The input being signed (replacing the scriptSig with scriptCode + amount)
//		// The prevout may already be contained in hashPrevout, and the nSequence
//		// may already be contain in hashSequence.
//		ss << tx.txIn(nIn).prevOut();
//		ss << scriptCode;
//		ss << amount;
//		ss << tx.txIn(nIn).sequence();
//		// Outputs (none/one/all, depending on flags)
//		ss << hashOutputs;
//#ifdef TKEYCOIN
//		ss << txTo.m_nSrcChain;
//		ss << txTo.m_nDestChain;
//#endif
//		// Locktime
//		ss << tx.tockTime();
//		// Sighash type
//		ss << nHashType;
//
//		return ss.GetHash();
//	}

	static const uint256 one(Hex::Parse("0000000000000000000000000000000000000000000000000000000000000001"));

	// Check for invalid use of SIGHASH_SINGLE
	if (maskedHashType.isSet(SignatureHashType::SIGHASH_SINGLE))
	{
		if (nIn >= tx.txOuts().size())
		{
			//  nOut out of range
			return one;
		}
	}

	// Wrapper to serialize only the necessary parts of the transaction being signed
	CTransactionSignatureSerializer<T> txTmp(tx, scriptCode, nIn, nHashType);

	// Serialize and hash
	CHashWriter ss(SerializationActionType::SER_GETHASH, 0);
	ss << txTmp << (Flags<SignatureHashType>::type)nHashType;
	return ss.GetHash();
}

template
uint256 SignatureHash<Transaction>(
	const Script& scriptCode,
	const Transaction& tx,
	unsigned int nIn,
	Flags<SignatureHashType> nHashType,
	const Amount& amount,
	SigVersion sigversion,
	const PrecomputedTransactionData* cache
);
