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


// Script.hpp

#pragma once

#include <cstdint>
#include <vector>
#include "../serialization/Serialization.hpp"
#include "OpCode.hpp"
#include "ScriptNum.hpp"
#include "../util/Endians.hpp"
#include "StackFrame.hpp"

using ScriptBase = std::vector<uint8_t>;

class Script : public ScriptBase, public Serializable
{
public:
	// Maximum number of bytes pushable to the stack
	static const unsigned int MAX_SCRIPT_ELEMENT_SIZE = 520;

	// Maximum number of non-push operations per script
	static const int MAX_OPS_PER_SCRIPT = 201;

	// Maximum number of public keys per multisig
	static const int MAX_PUBKEYS_PER_MULTISIG = 20;

	// Maximum script length in bytes
	static const int MAX_SCRIPT_SIZE = 10000;

	// Maximum number of values on script interpreter stack
	static const int MAX_STACK_SIZE = 1000;

	// Threshold for nLockTime: below this value it is interpreted as block number, otherwise as UNIX timestamp.
	static const unsigned int LOCKTIME_THRESHOLD = 500000000; // Tue Nov  5 00:53:20 1985 UTC

public:
	Script(Script&&) noexcept = default; // Move-constructor
	Script(const Script&) = default; // Copy-constructor
	~Script() = default; // Destructor
	Script& operator=(Script&&) noexcept = default; // Move-assignment
	Script& operator=(Script const&) = delete; // Copy-assignment

	Script() = default; // Default-constructor

	Script(const_iterator pbegin, const_iterator pend)
	: ScriptBase(pbegin, pend)
	{}

	Script(const unsigned char* pbegin, const unsigned char* pend)
	: ScriptBase(pbegin, pend)
	{}

	explicit Script(int64_t b)
	{
		operator<<(b);
	}

	explicit Script(OpCode b)
	{
		operator<<(b);
	}
	explicit Script(const ScriptNum& b)
	{
		operator<<(b);
	}

	explicit Script(const std::vector<unsigned char>& b)
	{
		operator<<(b);
	}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;

protected:
	Script& push_int64(int64_t n)
	{
		if (n == 0)
		{
			push_back(static_cast<value_type>(OpCode::OP_0));
		}
		else if (n == -1)
		{
			push_back(static_cast<value_type>(OpCode::OP_1NEGATE));
		}
		else if (n >= 1 && n <= 16)
		{
			push_back(n + (static_cast<value_type>(OpCode::OP_1) - 1));
		}
		else
		{
			ScriptNum num(n);
			*this << ScriptNum::serialize(n);
		}
		return *this;
	}

public:

	Script& operator+=(const Script& b)
	{
		reserve(size() + b.size());
		insert(end(), b.begin(), b.end());
		return *this;
	}

	friend Script operator+(const Script& a, const Script& b)
	{
		Script ret = a;
		ret += b;
		return ret;
	}

	Script& operator<<(int64_t number)
	{
		return push_int64(number);
	}

	Script& operator<<(OpCode opcode)
	{
		if (opcode < OpCode::OP_0 || opcode > OpCode::OP_INVALIDOPCODE)
		{
			throw std::invalid_argument("Script::operator<<(): invalid opcode");
		}
		emplace_back(static_cast<uint8_t>(opcode));
		return *this;
	}

	Script& operator<<(const ScriptNum& b)
	{
		*this << b.getvch();
		return *this;
	}

	Script& operator<<(const std::vector<uint8_t>& b)
	{
		if (b.size() < static_cast<size_t>(OpCode::OP_PUSHDATA1))
		{
			emplace_back(static_cast<uint8_t>(b.size()));
		}
		else if (b.size() <= 0xff)
		{
			emplace_back(static_cast<uint8_t>(OpCode::OP_PUSHDATA1));
			emplace_back(static_cast<uint8_t>(b.size()));
		}
		else if (b.size() <= 0xffff)
		{
			emplace_back(static_cast<uint8_t>(OpCode::OP_PUSHDATA2));

			uint8_t data[2];
			WriteLE16(data, b.size());
			insert(end(), data, data + sizeof(data));
		}
		else
		{
			emplace_back(static_cast<uint8_t>(OpCode::OP_PUSHDATA4));

			uint8_t data[4];
			WriteLE32(data, b.size());
			insert(end(), data, data + sizeof(data));
		}
		insert(end(), b.begin(), b.end());
		return *this;
	}

	Script& operator<<(std::basic_string_view<char> str)
	{
		if (str.size() < static_cast<size_t>(OpCode::OP_PUSHDATA1))
		{
			emplace_back(static_cast<uint8_t>(str.size()));
		}
		else if (str.size() <= 0xff)
		{
			emplace_back(static_cast<uint8_t>(OpCode::OP_PUSHDATA1));
			emplace_back(static_cast<uint8_t>(str.size()));
		}
		else if (str.size() <= 0xffff)
		{
			emplace_back(static_cast<uint8_t>(OpCode::OP_PUSHDATA2));

			uint8_t data[2];
			WriteLE16(data, str.size());
			insert(end(), data, data + sizeof(data));
		}
		else
		{
			emplace_back(static_cast<uint8_t>(OpCode::OP_PUSHDATA4));

			uint8_t data[4];
			WriteLE32(data, str.size());
			insert(end(), data, data + sizeof(data));
		}
		insert(end(), str.begin(), str.end());
		return *this;
	}

	Script& operator<<(const Script& b)
	{
		// I'm not sure if this should push the script or concatenate scripts.
		// If there's ever a use for pushing a script onto a script, delete this member fn
		assert(!"Warning: Pushing a Script onto a Script with << is probably not intended, use + to concatenate!");
		return *this;
	}

	bool GetOp(const_iterator& pc, OpCode& opcodeRet, std::vector<unsigned char>& vchRet) const
	{
		return GetScriptOp(pc, end(), opcodeRet, &vchRet);
	}

	bool GetOp(const_iterator& pc, OpCode& opcodeRet) const
	{
		return GetScriptOp(pc, end(), opcodeRet, nullptr);
	}

	/** Encode/decode small integers: */
	static int DecodeOP_N(OpCode opcode);
//	{
//		if (opcode == OpCode::OP_0)
//		{
//			return 0;
//		}
//		if (opcode < OpCode::OP_1 || opcode > OpCode::OP_16)
//		{
//			throw std::invalid_argument("Non numeric opcode");
//		}
//		return static_cast<uint8_t>(opcode) - static_cast<uint8_t>(OpCode::OP_1) + 1;
//	}

	static OpCode EncodeOP_N(int n);
//	{
//		if (n < 0 || n > 16)
//		{
//			throw std::invalid_argument("Number hasn't associated opcode");
//		}
//		if (n == 0)
//		{
//			return OpCode::OP_0;
//		}
//		return static_cast<OpCode>(static_cast<uint8_t>(OpCode::OP_1) + n - 1);
//	}

	/**
	 * Pre-version-0.6, Bitcoin always counted CHECKMULTISIGs
	 * as 20 sigops. With pay-to-script-hash, that changed:
	 * CHECKMULTISIGs serialized in scriptSigs are
	 * counted more accurately, assuming they are of the form
	 *  ... OpCode::OP_N CHECKMULTISIG ...
	 */
	unsigned int GetSigOpCount(bool fAccurate) const;

	/**
	 * Accurately count sigOps, including sigOps in
	 * pay-to-script-hash transactions:
	 */
	unsigned int GetSigOpCount(const Script& scriptSig) const;

	bool IsPayToScriptHash() const;
	bool IsPayToWitnessScriptHash() const;
	bool IsWitnessProgram(int& version, std::vector<unsigned char>& program) const;

	/** Called by IsStandardTx and P2SH/BIP62 VerifyScript (which makes it consensus-critical). */
	bool IsPushOnly(const_iterator pc) const;
	bool IsPushOnly() const;

	/** Check if the script contains valid OpCode::OP_CODES */
	bool HasValidOps() const;

	/**
	 * Returns whether the script is guaranteed to fail at execution,
	 * regardless of the initial stack. This allows outputs to be pruned
	 * instantly when entering the UTXO set.
	 */
	bool IsUnspendable() const;
//	{
//		return (!empty() && static_cast<OpCode>(front()) == OpCode::OP_RETURN) || (size() > MAX_SCRIPT_SIZE);
//	}

	void clear();
//	{
//		// The default prevector::clear() does not release memory
//		ScriptBase::clear();
//		shrink_to_fit();
//	}

	static bool GetScriptOp(ScriptBase::const_iterator& pc, ScriptBase::const_iterator end, OpCode& opcodeRet, std::vector<uint8_t>* pvchRet)
	{
		opcodeRet = OpCode::OP_INVALIDOPCODE;

		if (pvchRet)
		{
			pvchRet->clear();
		}

		if (pc >= end)
		{
			return false;
		}

		// Read instruction
		if (end - pc < 1)
		{
			return false;
		}

		OpCode opcode = static_cast<OpCode>(*pc++);

		// Immediate operand
		if (opcode <= OpCode::OP_PUSHDATA4)
		{
			size_t nSize = 0;
			if (opcode < OpCode::OP_PUSHDATA1)
			{
				nSize = static_cast<size_t>(opcode);
			}
			else if (opcode == OpCode::OP_PUSHDATA1)
			{
				if (end - pc < 1)
				{
					return false;
				}
				nSize = *pc++;
			}
			else if (opcode == OpCode::OP_PUSHDATA2)
			{
				if (end - pc < 2)
				{
					return false;
				}
				nSize = ReadLE16(&pc[0]);
				pc += 2;
			}
			else if (opcode == OpCode::OP_PUSHDATA4)
			{
				if (end - pc < 4)
				{
					return false;
				}
				nSize = ReadLE32(&pc[0]);
				pc += 4;
			}
			if (end - pc < 0 || (unsigned int)(end - pc) < nSize)
			{
				return false;
			}
			if (pvchRet)
			{
				pvchRet->assign(pc, pc + nSize);
			}
			pc += nSize;
		}

		opcodeRet = opcode;
		return true;
	}

	static void dump(std::ostream& os, const_iterator begin, const_iterator end)
	{
		auto i = begin;

		while (i != end)
		{
			OpCode opcode;
			ScriptData data;
			if (GetScriptOp(i, end, opcode, &data))
			{
				if (data.empty())
				{
					os << std::to_string(opcode);
				}
				else
				{
					os << std::hex << std::setfill('0');
					for (auto byte : data)
					{
						os << std::setw(2) << (uint)byte;
					}
					os << std::dec;
					try
					{
						ScriptNum n(data, true);
						os << '(' << n.getint() << ')';
					}
					catch (...) {}
				}
				os << ' ';
			}
			else
			{
				os << "!ERROR!";
				if (i != end)
				{
					os << " ...";
				}
				break;
			}
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const Script& script)
	{
		dump(os, script.begin(), script.end());
		return os;
	}
};

