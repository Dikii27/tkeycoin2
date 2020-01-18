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


// Flags.hpp

#pragma once

#include <cstddef>
#include <type_traits>
#include <cstdint>
#include <climits>
#include <utility>

namespace
{
template<size_t BITS>
struct helper
{
	template<size_t N> static typename std::enable_if_t<N == 1, uint8_t> detect();
	template<size_t N> static typename std::enable_if_t<N == 2, uint16_t> detect();
	template<size_t N> static typename std::enable_if_t<N == 4, uint32_t> detect();
	template<size_t N> static typename std::enable_if_t<N == 8, uint64_t> detect();

	using type = decltype(detect<
		(BITS <= 8 ) ? 1 :
		(BITS <= 16) ? 2 :
		(BITS <= 32) ? 4 :
		(BITS <= 64) ? 8 :
		(sizeof(uintmax_t) / CHAR_BIT)
	>());
};
}

template<typename FlagType>//, typename std::enable_if_t<std::is_integral_v<FlagType>, void>* = nullptr>
class Flags final
{
public:
	using type = typename helper<sizeof(FlagType)>::type;
private:
	type _flags;

public:
//	Flags(Flags&&) noexcept = delete; // Move-constructor
//	Flags(const Flags&) = default; // Copy-constructor
//	~Flags() = default; // Destructor
//	Flags& operator=(Flags&&) noexcept = delete; // Move-assignment
//	Flags& operator=(Flags const&) = default; // Copy-assignment

	Flags() // Default-constructor
	: _flags(0)
	{}

	Flags(type flags)
	: _flags(flags)
	{}

	template<typename... Args, typename std::enable_if<(sizeof...(Args) > 0), void>* = nullptr >
	bool set(FlagType flag, Args&&...args) const
	{
		_flags |= static_cast<type>(flag);
		if constexpr (sizeof...(args)>0)
		{
			return set(std::forward<Args>(args)...);
		}
	}

	template<typename... Args, typename std::enable_if<(sizeof...(Args) > 0), void>* = nullptr >
	bool unset(FlagType flag, Args&&...args) const
	{
		_flags &= ~static_cast<type>(flag);
		if constexpr (sizeof...(args)>0)
		{
			return set(std::forward<Args>(args)...);
		}
	}

	constexpr bool isSet(FlagType flag) const
	{
		return (_flags & static_cast<type>(flag)) != 0;
	}

	constexpr Flags mask(type mask) const
	{
		Flags res;
		res._flags &= mask;
		return res;
	}

	template<typename... Args, typename std::enable_if<(sizeof...(Args) > 0), void>* = nullptr >
	constexpr bool isSetAll(FlagType flag, Args&&...args) const
	{
		if (!isSet(flag))
		{
			return false;
		}
		if constexpr (sizeof...(args)>0)
		{
			return isSetAll(std::forward<Args>(args)...);
		}
		return true;
	}

	template<typename... Args, typename std::enable_if<(sizeof...(Args) > 0), void>* = nullptr >
	constexpr bool isSetAny(FlagType flag, Args&&...args) const
	{
		if (isSet(flag))
		{
			return true;
		}
		if constexpr (sizeof...(args)>0)
		{
			return isSetAny(std::forward<Args>(args)...);
		}
		return false;
	}

	Flags& operator+=(const FlagType& flag)
	{
		set(flag);
		return *this;
	}

	Flags& operator-=(const FlagType& flag)
	{
		unset(flag);
		return *this;
	}

	Flags& operator|=(const FlagType& flag)
	{
		set(flag);
		return *this;
	}

	explicit constexpr operator bool() const
	{
		return _flags > 0;
	}

	explicit constexpr operator type() const
	{
		return _flags;
	}

	friend Flags operator|(const Flags& flags, const FlagType& flag)
	{
		return flags._flags | static_cast<type>(flag);
	}

	friend Flags operator&(const Flags& flags, const FlagType& flag)
	{
		Flags res;
		res._flags = flags._flags & static_cast<type>(flag);
		return res;
	}
};
