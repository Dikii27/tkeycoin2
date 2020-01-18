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


// GenericTransactionSignatureChecker.cpp

#include <blockchain/TxIn.hpp>
#include <blockchain/Transaction.hpp>
#include "GenericTransactionSignatureChecker.hpp"
#include "SignatureHash.hpp"

template <class T>
bool GenericTransactionSignatureChecker<T>::VerifySignature(
	const std::vector<unsigned char>& vchSig,
	const CPubKey& pubkey,
	const uint256& sighash
) const
{
	return pubkey.Verify(sighash, vchSig);
}

template <class T>
bool GenericTransactionSignatureChecker<T>::CheckSig(
	const std::vector<unsigned char>& sigData,
	const std::vector<unsigned char>& pubKeyData,
	const Script& script,
	SigVersion sigversion
) const
{
	CPubKey pubkey(pubKeyData);
	if (!pubkey.IsValid())
	{
		return false;
	}

	// Hash type is one byte tacked on to the end of the signature
	std::vector<unsigned char> vchSig(sigData);
	if (vchSig.empty())
	{
		return false;
	}
	Flags<SignatureHashType> nHashType(vchSig.back());
	vchSig.pop_back();

	uint256 sighash = SignatureHash(script, *txTo, nIn, nHashType, amount, sigversion, this->txdata);

	if (!VerifySignature(vchSig, pubkey, sighash))
	{
		return false;
	}

	return true;
}

template <class T>
bool GenericTransactionSignatureChecker<T>::CheckLockTime(const CScriptNum& lockTime) const
{
	// There are two kinds of nLockTime: lock-by-blockheight
	// and lock-by-blocktime, distinguished by whether
	// nLockTime < LOCKTIME_THRESHOLD.
	//
	// We want to compare apples to apples, so fail the script
	// unless the type of nLockTime being tested is the same as
	// the nLockTime in the transaction.
	if (!(
		(txTo->lockTime() <  Script::LOCKTIME_THRESHOLD && lockTime <  Script::LOCKTIME_THRESHOLD) ||
		(txTo->lockTime() >= Script::LOCKTIME_THRESHOLD && lockTime >= Script::LOCKTIME_THRESHOLD)
	))
	{
		return false;
	}

	// Now that we know we're comparing apples-to-apples, the
	// comparison is a simple numeric one.
	if (lockTime > txTo->lockTime())
	{
		return false;
	}

	// Finally the nLockTime feature can be disabled and thus
	// CHECKLOCKTIMEVERIFY bypassed if every txin has been
	// finalized by setting nSequence to maxint. The
	// transaction would be allowed into the blockchain, making
	// the opcode ineffective.
	//
	// Testing if this vin is not final is sufficient to
	// prevent this condition. Alternatively we could test all
	// inputs, but testing just this input minimizes the data
	// required to prove correct CHECKLOCKTIMEVERIFY execution.
	if (TxIn::SEQUENCE_FINAL == txTo->txIn(nIn).sequence())
	{
		return false;
	}

	return true;
}

template <class T>
bool GenericTransactionSignatureChecker<T>::CheckSequence(const CScriptNum& nSequence) const
{
	// Relative lock times are supported by comparing the passed
	// in operand to the sequence number of the input.
	const int64_t txToSequence = (int64_t)txTo->txIn(nIn).sequence();

	// Fail if the transaction's version number is not set high
	// enough to trigger BIP 68 rules.
	if (static_cast<uint32_t>(txTo->version()) < 2)
	{
		return false;
	}

	// Sequence numbers with their most significant bit set are not
	// consensus constrained. Testing that the transaction's sequence
	// number do not have this bit set prevents using this property
	// to get around a CHECKSEQUENCEVERIFY check.
	if (txToSequence & TxIn::SEQUENCE_LOCKTIME_DISABLE_FLAG)
	{
		return false;
	}

	// Mask off any bits that do not have consensus-enforced meaning
	// before doing the integer comparisons
	const uint32_t nLockTimeMask = TxIn::SEQUENCE_LOCKTIME_TYPE_FLAG | TxIn::SEQUENCE_LOCKTIME_MASK;
	const int64_t txToSequenceMasked = txToSequence & nLockTimeMask;
	const CScriptNum nSequenceMasked = nSequence & nLockTimeMask;

	// There are two kinds of nSequence: lock-by-blockheight
	// and lock-by-blocktime, distinguished by whether
	// nSequenceMasked < TxIn::SEQUENCE_LOCKTIME_TYPE_FLAG.
	//
	// We want to compare apples to apples, so fail the script
	// unless the type of nSequenceMasked being tested is the same as
	// the nSequenceMasked in the transaction.
	if (!(
		(txToSequenceMasked <  TxIn::SEQUENCE_LOCKTIME_TYPE_FLAG && nSequenceMasked <  TxIn::SEQUENCE_LOCKTIME_TYPE_FLAG) ||
		(txToSequenceMasked >= TxIn::SEQUENCE_LOCKTIME_TYPE_FLAG && nSequenceMasked >= TxIn::SEQUENCE_LOCKTIME_TYPE_FLAG)
	))
	{
		return false;
	}

	// Now that we know we're comparing apples-to-apples, the
	// comparison is a simple numeric one.
	if (nSequenceMasked > txToSequenceMasked)
	{
		return false;
	}

	return true;
}

// explicit instantiation
template class GenericTransactionSignatureChecker<Transaction>;
