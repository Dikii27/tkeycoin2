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


// TxIn.hpp

#pragma once

#include "TxOutPoint.hpp"
#include "../types/VariableLengthInteger.hpp"
#include "../script/Script.hpp"
#include "TxWitness.hpp"

class TxIn : public Serializable
{
private:
	TxOutPoint _prevOut; //	The previous output transaction reference, as an OutPoint structure
	Script _sigScript; // Computational Script for confirming transaction authorization
	uint32_t _sequence; // Transaction version as defined by the sender. Intended for "replacement" of transactions when information is updated before inclusion into a block.
	TxWitness _scriptWitness;

public:
	/* Setting nSequence to this value for every input in a transaction
    * disables nLockTime. */
	static const uint32_t SEQUENCE_FINAL = 0xffffffff; // TODO ?

	/* Below flags apply in the context of BIP 68*/
	/* If this flag set, CTxIn::nSequence is NOT interpreted as a
	 * relative lock-time. */
	static const uint32_t SEQUENCE_LOCKTIME_DISABLE_FLAG = (1 << 31); // TODO ?

	/* If CTxIn::nSequence encodes a relative lock-time and this flag
	 * is set, the relative lock-time has units of 512 seconds,
	 * otherwise it specifies blocks with a granularity of 1. */
	static const uint32_t SEQUENCE_LOCKTIME_TYPE_FLAG = (1 << 22); // TODO ?

	/* If CTxIn::nSequence encodes a relative lock-time, this mask is
	 * applied to extract that lock-time from the sequence field. */
	static const uint32_t SEQUENCE_LOCKTIME_MASK = 0x0000ffff; // TODO ?

	/* In order to use the same number of bits to encode roughly the
	 * same wall-clock duration, and because blocks are naturally
	 * limited to occur every 600s on average, the minimum granularity
	 * for time-based relative lock-time is fixed at 512 seconds.
	 * Converting from CTxIn::nSequence to seconds is performed by
	 * multiplying by 512 = 2^9, or equivalently shifting up by
	 * 9 bits. */
	static const int SEQUENCE_LOCKTIME_GRANULARITY = 9; // TODO ?

public:
	TxIn(TxIn&&) noexcept = default; // Move-constructor
	TxIn(const TxIn&) = default; // Copy-constructor
	~TxIn() = default; // Destructor
	TxIn& operator=(TxIn&&) noexcept = delete; // Move-assignment
	TxIn& operator=(TxIn const&) = delete; // Copy-assignment

	TxIn() // Default-constructor
	: _sequence(std::numeric_limits<decltype(_sequence)>::max())
	{
	}

	TxIn(TxOutPoint&& prevOut, Script&& sigScript, uint32_t sequence = 0)
	: _prevOut(std::forward<TxOutPoint>(prevOut))
	, _sigScript(std::forward<Script>(sigScript))
	, _sequence(sequence)
	{
	}

	TxIn(uint256&& prevOutHash, uint32_t prevOutIndex, Script&& sigScript, uint32_t sequence = 0)
	: _prevOut(std::forward<uint256>(prevOutHash), prevOutIndex)
	, _sigScript(std::forward<Script>(sigScript))
	, _sequence(sequence)
	{
	}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;

	[[nodiscard]]
	const TxOutPoint& prevOut() const
	{
		return _prevOut;
	}

	[[nodiscard]]
	auto sequence() const
	{
		return _sequence;
	}

	[[nodiscard]]
	const TxWitness& scriptWitness() const
	{
		return _scriptWitness;
	}

	TxWitness& scriptWitness()
	{
		return _scriptWitness;
	}
};
