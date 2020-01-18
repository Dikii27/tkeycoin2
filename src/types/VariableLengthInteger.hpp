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


// VariableLengthInteger.hpp

#pragma once

#include <cstdint>
#include "../serialization/Serialization.hpp"

class VariableLengthInteger : public Serializable
{
private:
	size_t _value;

public:
	VariableLengthInteger(VariableLengthInteger&&) noexcept = default; // Move-constructor
	VariableLengthInteger(const VariableLengthInteger&) = default; // Copy-constructor
	~VariableLengthInteger() = default; // Destructor
	VariableLengthInteger& operator=(VariableLengthInteger&&) noexcept = default; // Move-assignment
	VariableLengthInteger& operator=(VariableLengthInteger const&) = default; // Copy-assignment

	VariableLengthInteger() // Default-constructor
	: _value(0)
	{}

	VariableLengthInteger(size_t value)
	: _value(value)
	{}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override
	{
		return _value;
	}

	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	explicit operator T() const { return _value; }

	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	bool operator<(const T& value) const { return _value < value; }
	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	bool operator<=(const T& value) const { return _value <= value; }
	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	bool operator==(const T& value) const { return _value == value; }
	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	bool operator>=(const T& value) const { return _value >= value; }
	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	bool operator>(const T& value) const { return _value > value; }
	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	bool operator!=(const T& value) const { return _value != value; }

	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	VariableLengthInteger& operator-(const T& value) { _value -= value; return *this; }
	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	VariableLengthInteger& operator+(const T& value) { _value += value; return *this; }

	VariableLengthInteger& operator--() { --_value; return *this; }
	VariableLengthInteger& operator++() { ++_value; return *this; }

	VariableLengthInteger operator--(int) { auto tmp = *this; --_value; return tmp; }
	VariableLengthInteger operator++(int) { auto tmp = *this; ++_value; return tmp; }

	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	friend bool operator<(const T& lhs, const VariableLengthInteger& rhs) { return lhs < rhs._value; }
};

using uintV = VariableLengthInteger;

