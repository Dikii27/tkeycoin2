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


// StackFrame.hpp

#pragma once

#include <cstdint>
#include <vector>
#include <util/Hex.hpp>
#include <iomanip>
#include <algorithm>
#include <variant>
#include "ScriptNum.hpp"
#include "ScriptData.hpp"

class StackFrame : private std::variant<ScriptNum, ScriptData>
{
public:
	using Number = ScriptNum;
	using Data = ScriptData;

	StackFrame() = default; // Default-constructor
	~StackFrame() = default; // Destructor
	StackFrame& operator=(StackFrame&&) noexcept = default; // Move-assignment
	StackFrame& operator=(StackFrame const&) = default; // Copy-assignment

	StackFrame(StackFrame&& other) noexcept // Move-constructor
	{
		if (other.isNumber())
		{
			*this = Number(std::forward<Number>(other.asNumber()));
		}
		else if (other.isData())
		{
			*this = Data(std::forward<Data>(other.asData()));
		}
	}
	StackFrame(const StackFrame& other) // Copy-constructor
	{
		if (other.isNumber())
		{
			*this = Number(other.asNumber(false));
		}
		else if (other.isData())
		{
			*this = Data(other.asData());
		}
	}

	template <typename T>
	StackFrame(T&& a)
	: variant(std::forward<T>(a))
	{
	}

	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	explicit StackFrame(T number)
	: variant(Number(number))
	{
	}

	StackFrame(Data&& data)
	: variant(Data(std::move(data)))
	{
	}

	StackFrame(const Data& data)
	: variant(Data(data))
	{
	}

	[[nodiscard]]
	constexpr bool isData() const noexcept
	{
		return std::holds_alternative<Data>(*this);
	}

	[[nodiscard]]
	const Data& asData() const
	{
		return std::get<Data>(*this);
	}

	Data& asData()
	{
		if (auto number = std::get_if<Number>(this))
		{
			*this = ScriptData(number->getvch());
		}

		return std::get<Data>(*this);
	}

	[[nodiscard]]
	constexpr bool isNumber() const noexcept
	{
		return std::holds_alternative<Number>(*this);
	}

	[[nodiscard]]
	const Number& asNumber(bool = false) const
	{
		return std::get<Number>(*this);
	}

	Number& asNumber(bool requireMinimizeData = false)
	{
		if (auto data = std::get_if<Data>(this))
		{
			*this = ScriptNum(*data, requireMinimizeData);
		}

		return std::get<Number>(*this);
	}

	constexpr bool operator==(const StackFrame& other) const
	{
		if (isNumber() && other.isNumber())
		{
			return bool(asNumber(false) == other.asNumber(false));
		}
		else if (isData() && other.isData())
		{
			return asData() == other.asData();
		}
		return false;
	}

	friend std::ostream& operator<<(std::ostream& os, const StackFrame& stackFrame)
	{
		if (stackFrame.isNumber())
		{
			return os << stackFrame.asNumber(false).getint();
		}
		else if (stackFrame.isData())
		{
			os << std::hex << std::setfill('0');
			std::for_each(stackFrame.asData().cbegin(), stackFrame.asData().cend(), [&os](auto& byte){os << std::setw(2) << (uint)byte;});
			return os;
		}
		else
		{
			os << "UNKNOWN";
			return os;
		}
	}
};
