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


// TransactionSignatureSerializer.hpp

#pragma once

//
//class TransactionSignatureSerializer final
//{
//public:
//	TransactionSignatureSerializer() = delete; // Default-constructor
//	TransactionSignatureSerializer(TransactionSignatureSerializer&&) noexcept = delete; // Move-constructor
//	TransactionSignatureSerializer(const TransactionSignatureSerializer&) = delete; // Copy-constructor
//	virtual ~TransactionSignatureSerializer() override = default; // Destructor
//	TransactionSignatureSerializer& operator=(TransactionSignatureSerializer&&) noexcept = delete; // Move-assignment
//	TransactionSignatureSerializer& operator=(TransactionSignatureSerializer const&) = delete; // Copy-assignment
//
//	TransactionSignatureSerializer()
//	{}
//};


#include <script/Script.hpp>
#include <blockchain/TxOut.hpp>
#include <types/Flags.hpp>
#include <blockchain/Transaction.hpp>
#include "SignatureHashType.hpp"

/**
 * Wrapper that serializes like CTransaction, but with the modifications
 *  required for the signature hash done in-place
 */
template<class T>
//using T = Transaction;
class CTransactionSignatureSerializer : public Serializable
{
private:
	const T& txTo;             //!< reference to the spending transaction (the one being serialized)
	const Script& scriptCode; //!< output script being consumed
	const unsigned int nIn;    //!< input index of txTo being signed
	const bool fAnyoneCanPay;  //!< whether the hashtype has the SIGHASH_ANYONECANPAY flag set
	const bool fHashSingle;    //!< whether the hashtype is SIGHASH_SINGLE
	const bool fHashNone;      //!< whether the hashtype is SIGHASH_NONE

public:
	CTransactionSignatureSerializer(
		const T& txToIn,
		const Script& scriptCodeIn,
		unsigned int nInIn,
		Flags<SignatureHashType> nHashTypeIn
	)
	: txTo(txToIn)
	, scriptCode(scriptCodeIn)
	, nIn(nInIn)
	, fAnyoneCanPay(nHashTypeIn.isSet(SignatureHashType::SIGHASH_ANYONECANPAY))
	, fHashSingle(nHashTypeIn.mask(0x1f).isSet(SignatureHashType::SIGHASH_SINGLE))
	, fHashNone(nHashTypeIn.mask(0x1f).isSet(SignatureHashType::SIGHASH_NONE))
	{
	}

	/** Serialize the passed scriptCode, skipping OP_CODESEPARATORs */
	template<typename S>
	void SerializeScriptCode(S& s) const
	{
		Script::const_iterator it = scriptCode.begin();
		Script::const_iterator itBegin = it;
		OpCode opcode;
		unsigned int nCodeSeparators = 0;
		while (scriptCode.GetOp(it, opcode))
		{
			if (opcode == OpCode::OP_CODESEPARATOR)
			{
				nCodeSeparators++;
			}
		}

		uintV len = scriptCode.size() - nCodeSeparators;
		len.Serialize(s);
//		::WriteCompactSize(s, scriptCode.size() - nCodeSeparators);

		it = itBegin;
		while (scriptCode.GetOp(it, opcode))
		{
			if (opcode == OpCode::OP_CODESEPARATOR)
			{
				s.write((char*) &itBegin[0], it - itBegin - 1);
				itBegin = it;
			}
		}
		if (itBegin != scriptCode.end())
		{
			s.write((char*) &itBegin[0], it - itBegin);
		}
	}

	/** Serialize an input of txTo */
	template<typename S>
	void SerializeInput(S& s, unsigned int nInput) const
	{
		// In case of SIGHASH_ANYONECANPAY, only the input being signed is serialized
		if (fAnyoneCanPay)
		{
			nInput = nIn;
		}
		// Serialize the prevout
		::Serialize(s, txTo.txIn(nInput).prevOut());
		// Serialize the script
		if (nInput != nIn)
		{
			// Blank out other inputs' signatures
			::Serialize(s, Script());
		}
		else
		{
			SerializeScriptCode(s);
		}
		// Serialize the nSequence
		if (nInput != nIn && (fHashSingle || fHashNone))
		{
			// let the others update at will
			::Serialize(s, (int) 0);
		}
		else
		{
			::Serialize(s, txTo.txIn(nInput).sequence());
		}
	}

	/** Serialize an output of txTo */
	template<typename S>
	void SerializeOutput(S& s, unsigned int nOutput) const
	{
		if (fHashSingle && nOutput != nIn)
		{
			// Do not lock-in the txout payee at other indices as txin
			::Serialize(s, TxOut());
		}
		else
		{
			::Serialize(s, txTo.txOut(nOutput));
		}
	}

	/** Serialize txTo */
	void Serialize(std::ostream& s) const override
	{
		// Serialize nVersion
		::Serialize(s, txTo.version());
		// Serialize vin
		unsigned int nInputs = fAnyoneCanPay ? 1 : txTo.txIns().size();

		uintV len = nInputs;
		len.Serialize(s);
//		::WriteCompactSize(s, nInputs);


		for (unsigned int nInput = 0; nInput < nInputs; nInput++)
		{
			SerializeInput(s, nInput);
		}
		// Serialize vout
		unsigned int nOutputs = fHashNone ? 0 : (fHashSingle ? nIn + 1 : txTo.txOuts().size());

		len = nOutputs;
		len.Serialize(s);
//		::WriteCompactSize(s, nOutputs);

		for (unsigned int nOutput = 0; nOutput < nOutputs; nOutput++)
		{
			SerializeOutput(s, nOutput);
		}
#ifdef TKEYCOIN
		::Serialize(s, txTo.m_nSrcChain);
		::Serialize(s, txTo.m_nDestChain);
#endif
		// Serialize nLockTime
		::Serialize(s, txTo.lockTime());
	}

	void Unserialize(std::istream& is) override
	{

	}

	[[nodiscard]]
	SVal toSVal() const override { throw std::runtime_error("This is code must never calling"); }
};
