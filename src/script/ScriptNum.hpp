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


// ScriptNum.hpp

#pragma once

#include <cstdint>
#include <limits>
#include <cassert>
#include <vector>
#include <stdexcept>
#include <serialization/Serialization.hpp>
#include "ScriptException.hpp"

/**
 * Numeric opcodes (OP_1ADD, etc) are restricted to operating on 4-byte integers.
 * The semantics are subtle, though: operands must be in the range [-2^31 +1...2^31 -1],
 * but results may overflow (and are valid as long as they are not used in a subsequent
 * numeric operation). ScriptNum enforces those semantics by storing results as
 * an int64 and allowing out-of-range values to be returned as a vector of bytes but
 * throwing an exception if arithmetic is done or the result is interpreted as an integer.
 */

class ScriptNum : public Serializable
{
private:
	using inner_type = int64_t;
	using outer_type = int32_t;

	static constexpr bool requireMinimalSize = false;

	inner_type _value;

public:
//	ScriptNum() = default; // Default-constructor
	ScriptNum(ScriptNum&&) noexcept = default; // Move-constructor
	ScriptNum(const ScriptNum&) = default; // Copy-constructor
	~ScriptNum() = default; // Destructor
	ScriptNum& operator=(ScriptNum&&) noexcept = default; // Move-assignment
	ScriptNum& operator=(ScriptNum const&) = default; // Copy-assignment

public:

	ScriptNum()
	: _value(0)
	{
	}

	explicit ScriptNum(const int64_t& n)
	: _value(n)
	{
	}

	static const size_t nDefaultMaxNumSize = 4;

	explicit ScriptNum(
		const std::vector<uint8_t>& vch,
		bool fRequireMinimal,
		const size_t nMaxNumSize = sizeof(outer_type)
	)
	{
		if (vch.size() > nMaxNumSize)
		{
			throw ScriptException(ScriptError::CONVERTION_TO_NUMBER, "script number overflow");
		}
		if (fRequireMinimal && !vch.empty())
		{
			// Check that the number is encoded with the minimum possible
			// number of bytes.
			//
			// If the most-significant-byte - excluding the sign bit - is zero
			// then we're not minimal. Note how this test also rejects the
			// negative-zero encoding, 0x80.
			if ((vch.back() & 0x7f) == 0)
			{
				// One exception: if there's more than one byte and the most
				// significant bit of the second-most-significant-byte is set
				// it would conflict with the sign bit. An example of this case
				// is +-255, which encode to 0xff00 and 0xff80 respectively.
				// (big-endian).
				if (vch.size() <= 1 || (vch[vch.size() - 2] & 0x80) == 0)
				{
					throw ScriptException(ScriptError::CONVERTION_TO_NUMBER, "non-minimally encoded script number");
				}
			}
		}
		_value = set_vch(vch);
	}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override
	{
		return _value;
	}

	inline bool operator==(const int64_t& rhs) const { return _value == rhs; }
	inline bool operator!=(const int64_t& rhs) const { return _value != rhs; }
	inline bool operator<=(const int64_t& rhs) const { return _value <= rhs; }
	inline bool operator<(const int64_t& rhs) const { return _value < rhs; }
	inline bool operator>=(const int64_t& rhs) const { return _value >= rhs; }
	inline bool operator>(const int64_t& rhs) const { return _value > rhs; }

	inline ScriptNum operator+(const int64_t& rhs) const { return ScriptNum(_value + rhs); }
	inline ScriptNum operator-(const int64_t& rhs) const { return ScriptNum(_value - rhs); }

	inline ScriptNum operator&(const int64_t& rhs) const { return ScriptNum(_value & rhs); }

	inline bool operator==(const ScriptNum& rhs) const { return operator==(rhs._value); }
	inline bool operator!=(const ScriptNum& rhs) const { return operator!=(rhs._value); }
	inline bool operator<=(const ScriptNum& rhs) const { return operator<=(rhs._value); }
	inline bool operator<(const ScriptNum& rhs) const { return operator<(rhs._value); }
	inline bool operator>=(const ScriptNum& rhs) const { return operator>=(rhs._value); }
	inline bool operator>(const ScriptNum& rhs) const { return operator>(rhs._value); }

	inline ScriptNum operator+(const ScriptNum& rhs) const { return operator+(rhs._value); }
	inline ScriptNum operator-(const ScriptNum& rhs) const { return operator-(rhs._value); }

	inline ScriptNum& operator+=(const ScriptNum& rhs) { return operator+=(rhs._value); }
	inline ScriptNum& operator-=(const ScriptNum& rhs) { return operator-=(rhs._value); }

	inline ScriptNum operator&(const ScriptNum& rhs) const { return operator&(rhs._value); }

	inline ScriptNum& operator&=(const ScriptNum& rhs) { return operator&=(rhs._value); }

	inline ScriptNum operator-() const
	{
		if (_value == std::numeric_limits<int64_t>::min())
		{
			throw ScriptException(ScriptError::NUMERIC_OPERATION, "result of operator- great that max");
		}
		return ScriptNum(-_value);
	}

	inline ScriptNum& operator=(const int64_t& rhs)
	{
		_value = rhs;
		return *this;
	}

	inline ScriptNum& operator+=(const int64_t& rhs)
	{
		if (rhs > 0 && _value > std::numeric_limits<int64_t>::max() - rhs)
		{
			throw ScriptException(ScriptError::NUMERIC_OPERATION, "result of operator+= less that min");
		}
		if (rhs < 0 && _value < std::numeric_limits<int64_t>::min() - rhs)
		{
			throw ScriptException(ScriptError::NUMERIC_OPERATION, "result of operator+= great that max");
		}
		_value += rhs;
		return *this;
	}

	inline ScriptNum& operator-=(const int64_t& rhs)
	{
		if (rhs > 0 && _value < std::numeric_limits<int64_t>::min() + rhs)
		{
			throw ScriptException(ScriptError::NUMERIC_OPERATION, "result of operator-= less that min");
		}
		if (rhs < 0 && _value > std::numeric_limits<int64_t>::max() + rhs)
		{
			throw ScriptException(ScriptError::NUMERIC_OPERATION, "result of operator-= more that min");
		}
		_value -= rhs;
		return *this;
	}

	inline ScriptNum& operator&=(const int64_t& rhs)
	{
		_value &= rhs;
		return *this;
	}

	template <typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	inline explicit operator T() const
	{
		return _value;
	}

	inline explicit operator bool() const
	{
		return _value != 0;
	}


	[[nodiscard]]
	outer_type getint() const
	{
		if (_value >= std::numeric_limits<outer_type>::max())
		{
			return std::numeric_limits<outer_type>::max();
		}
		else if (_value <= std::numeric_limits<outer_type>::min())
		{
			return std::numeric_limits<outer_type>::min();
		}
		return _value;
	}

	[[nodiscard]]
	std::vector<uint8_t> getvch() const // TODO remove
	{
		return serialize(_value);
	}

	static std::vector<uint8_t> serialize(const int64_t& value)
	{
		std::vector<uint8_t> result;

		if (value == 0)
		{
			return result;
		}

		const bool neg = value < 0;
		uint64_t absvalue = neg ? -value : value;

		while (absvalue)
		{
			result.push_back(absvalue & 0xffu);
			absvalue >>= 8u;
		}

//		- If the most significant byte is >= 0x80 and the value is positive, push a
//		new zero-byte to make the significant byte < 0x80 again.
//
//		- If the most significant byte is >= 0x80 and the value is negative, push a
//		new 0x80 byte that will be popped off when converting to an integral.
//
//		- If the most significant byte is < 0x80 and the value is negative, add
//		0x80 to it, since it will be subtracted and interpreted as a negative when
//		converting to an integral.

		if (result.back() & 0x80u)
		{
			result.push_back(neg ? 0x80 : 0);
		}
		else if (neg)
		{
			result.back() |= 0x80u;
		}

		return result;
	}

private:
	static int64_t set_vch(const std::vector<uint8_t>& vch)
	{
		if (vch.empty())
		{
			return 0;
		}

		int64_t result = 0;
		for (size_t i = 0; i != vch.size(); ++i)
		{
			result |= static_cast<int64_t>(vch[i]) << 8 * i;
		}

		// If the input vector's most significant byte is 0x80, remove it from
		// the result's msb and return a negative.

		if (vch.back() & 0x80u)
		{
			return -(static_cast<int64_t>(result & ~(0x80ull << (8 * (vch.size() - 1)))));
		}

		return result;
	}
};

using CScriptNum = ::ScriptNum;
