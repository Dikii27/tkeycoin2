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


// Transaction.hpp

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "../types/VariableLengthInteger.hpp"
#include "TxIn.hpp"
#include "TxOut.hpp"
#include "TxWitness.hpp"

class Transaction : public Serializable
{
private:
	int32_t _version = 0;                   // Transaction data format version (note, this is signed)
	std::optional<uint16_t> _flag;      // If present, always 0001, and indicates the presence of witness data
	std::vector<TxIn> _txIn;            // A list of 1 or more transaction inputs or sources for coins
	std::vector<TxOut> _txOut;          // A list of 1 or more transaction outputs or destinations for coins
//	std::vector<TxWitness> _txWitness;  // A list of witnesses, one for each input; omitted if flag is omitted above

	uint32_t _srcChain = -1;
	uint32_t _dstChain = -1;

	uint32_t _lockTime = 0;                 // The block number or timestamp at which this transaction is unlocked:

//	Value	Description
//0	Not locked
//< 500000000	Block number at which this transaction is unlocked
//>= 500000000	UNIX timestamp at which this transaction is unlocked
//If all TxIn inputs have final (0xffffffff) sequence numbers then lock_time is irrelevant. Otherwise, the transaction may not be added to a block until after lock_time (see NLockTime).

	mutable uint256 _hash;
	mutable bool _validHash = 0;

public:
	Transaction() = default; // Default-constructor
	Transaction(Transaction&&) noexcept = delete; // Move-constructor
	Transaction(const Transaction&) = delete; // Copy-constructor
	~Transaction() = default; // Destructor
	Transaction& operator=(Transaction&&) noexcept = delete; // Move-assignment
	Transaction& operator=(Transaction const&) = delete; // Copy-assignment


	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;

	[[nodiscard]]
	const uint256& hash() const;

	[[nodiscard]]
	bool HasWitness() const
	{
		for (auto& txIn : _txIn)
		{
			if (!txIn.scriptWitness().IsNull())
			{
				return true;
			}
		}
		return false;
	}

	[[nodiscard]]
	auto version() const
	{
		return _version;
	}

	[[nodiscard]]
	auto lockTime() const
	{
		return _lockTime;
	}

	[[nodiscard]]
	auto& txIn(size_t index) const
	{
		return _txIn.at(index);
	}

	[[nodiscard]]
	const auto& txIns() const
	{
		return _txIn;
	}

	[[nodiscard]]
	auto& txOut(size_t index) const
	{
		return _txOut.at(index);
	}

	[[nodiscard]]
	const auto& txOuts() const
	{
		return _txOut;
	}
};


