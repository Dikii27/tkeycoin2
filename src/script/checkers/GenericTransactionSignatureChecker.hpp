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


// GenericTransactionSignatureChecker.hpp

#pragma once


//class GenericTransactionSignatureChecker final
//{
//public:
//	GenericTransactionSignatureChecker() = delete; // Default-constructor
//	GenericTransactionSignatureChecker(GenericTransactionSignatureChecker&&) noexcept = delete; // Move-constructor
//	GenericTransactionSignatureChecker(const GenericTransactionSignatureChecker&) = delete; // Copy-constructor
//	virtual ~GenericTransactionSignatureChecker() override = default; // Destructor
//	GenericTransactionSignatureChecker& operator=(GenericTransactionSignatureChecker&&) noexcept = delete; // Move-assignment
//	GenericTransactionSignatureChecker& operator=(GenericTransactionSignatureChecker const&) = delete; // Copy-assignment
//
//	GenericTransactionSignatureChecker()
//	{}
//};


#include <blockchain/Amount.hpp>
#include <crypto/keys/CPubKey.hpp>
#include <utility>
#include "SignatureChecker.hpp"
#include "PrecomputedTransactionData.hpp"

template <class T>
class GenericTransactionSignatureChecker : public BaseSignatureChecker
{
private:
	const T* txTo;
	unsigned int nIn;
	const Amount amount;
	const PrecomputedTransactionData* txdata;

protected:
	[[nodiscard]]
	virtual bool VerifySignature(
		const std::vector<unsigned char>& vchSig,
		const CPubKey& vchPubKey,
		const uint256& sighash
	) const;

public:
	GenericTransactionSignatureChecker()
	: txTo(nullptr)
	, nIn(0)
	, amount(0)
	, txdata(nullptr)
	{
	}

	GenericTransactionSignatureChecker(
		const T* txToIn,
		unsigned int nInIn,
		Amount amountIn
	)
	: txTo(txToIn)
	, nIn(nInIn)
	, amount(std::move(amountIn))
	, txdata(nullptr)
	{
	}

	GenericTransactionSignatureChecker(
		const T* txToIn,
		unsigned int nInIn,
		Amount amountIn,
		const PrecomputedTransactionData& txdataIn
	)
	: txTo(txToIn)
	, nIn(nInIn)
	, amount(std::move(amountIn))
	, txdata(&txdataIn)
	{
	}

	[[nodiscard]]
	bool CheckSig(
		const std::vector<unsigned char>& sigData,
		const std::vector<unsigned char>& pubKeyData,
		const Script& script,
		SigVersion sigversion
	) const override;

	[[nodiscard]]
	bool CheckLockTime(const CScriptNum& lockTime) const override;

	[[nodiscard]]
	bool CheckSequence(const CScriptNum& nSequence) const override;
};

//using TransactionSignatureChecker = GenericTransactionSignatureChecker<CTransaction>;
//using MutableTransactionSignatureChecker = GenericTransactionSignatureChecker<CMutableTransaction>;
