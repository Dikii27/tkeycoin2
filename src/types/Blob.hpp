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


// BaseBlob.hpp

#pragma once


#include <cstdint>
#include <climits>
#include <array>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <type_traits>
#include "../util/Hex.hpp"
#include "../serialization/Serialization.hpp"

template<size_t BITS>
class Blob : public std::array<uint8_t, (BITS + CHAR_BIT - 1) / CHAR_BIT>, public Serializable
{
public:
	static_assert(BITS > 0);
	static constexpr size_t bits = BITS;
	static constexpr size_t bytes = (BITS + CHAR_BIT - 1) / CHAR_BIT;

protected:
	using base = std::array<uint8_t, bytes>;

public:
	Blob(Blob&&) noexcept = default; // Move-constructor
	Blob(const Blob&) = default; // Copy-constructor
	~Blob() = default; // Destructor
	Blob& operator=(Blob&&) noexcept = default; // Move-assignment
	Blob& operator=(Blob const&) = default; // Copy-assignment

	Blob() // Default-constructor
	{
		setNull();
	}

	template<
	    typename T,
	    typename std::enable_if_t<
	        !std::is_constructible_v<std::string_view, T> &&
	        std::is_integral_v<typename T::value_type> &&
	        sizeof(typename T::value_type) == 1, void
	    >* = nullptr
	>
	Blob(const T& data) // Constructor by vector or array of bytes
	{
		auto size = std::min(data.rend() - data.rbegin(), base::end() - base::begin());
		auto i = std::copy(data.rbegin(), data.rbegin() + size, base::begin());
		std::fill(i, base::end(), 0);
		for (auto ii = data.begin() + size; ii != data.end(); ++ii)
		{
			if (*ii) throw std::overflow_error("Inner data overflows destination");
		}
	}

	template<
		typename T,
		typename std::enable_if_t<std::is_constructible_v<std::string_view, T>, void>* = nullptr
	>
	Blob(const T& data) // Constructor by various string views
	{
		std::string_view str(data);

		auto srcIt = str.cbegin();

		// skip leading spaces
		while (std::isspace(*srcIt)) ++srcIt;

		// skip 0x
		if (*srcIt == '0')
		{
			++srcIt;
			if (*srcIt == 'x' || *srcIt == 'X')
			{
				++srcIt;
			}
			else
			{
				--srcIt;
			}
		}

		// skip leading zeros
		while (*srcIt == '0') ++srcIt;

		auto last = srcIt;

		// find end of hex-digits sequence
		srcIt = std::find_if(srcIt, str.cend(), [](auto x){return !Hex::isDigit(x);}) - 1;

		// fill the blob
		for (auto dsrIt = base::begin(); dsrIt != base::end(); ++dsrIt)
		{
			if (srcIt < last)
			{
				std::fill(dsrIt, base::end(), 0);
				break;
			}

			*dsrIt = Hex::digitToInt(*srcIt--);
			if (srcIt >= last)
			{
				*dsrIt |= Hex::digitToInt(*srcIt--) << 4u;
			}
		}
	}

	[[nodiscard]]
	bool isNull() const
	{
		return std::find_if(base::cbegin(), base::cend(), [](auto byte){return byte;}) == base::end();
	}

	void setNull()
	{
		std::fill_n(base::begin(), base::size(), 0);
	}

	[[nodiscard]]
	std::string str() const
	{
		std::ostringstream oss;
		oss << std::hex << std::setfill('0');
		std::for_each(base::crbegin(), base::crend(), [&oss](auto& byte) { oss << std::setw(2) << (uint) byte; });
		return oss.str();
	}

	[[nodiscard]]
	uint64_t GetUint64(int pos) const
	{
		const uint8_t* ptr = base::data() + pos * 8;
		return ((uint64_t)ptr[0]) | \
               ((uint64_t)ptr[1]) << 8u | \
               ((uint64_t)ptr[2]) << 16u | \
               ((uint64_t)ptr[3]) << 24u | \
               ((uint64_t)ptr[4]) << 32u | \
               ((uint64_t)ptr[5]) << 40u | \
               ((uint64_t)ptr[6]) << 48u | \
               ((uint64_t)ptr[7]) << 56u;
	}

	void Serialize(std::ostream& os) const override
	{
		os.write(reinterpret_cast<const char*>(base::data()), base::size());
	}

	void Unserialize(std::istream& is) override
	{
		is.read(reinterpret_cast<char*>(base::data()), base::size());
	}

	[[nodiscard]]
	SVal toSVal() const override
	{
		return str();
		return SBinary(base::data(), base::size());
	}

	explicit operator bool() const
	{
		return !isNull();
	}

	friend std::ostream& operator<<(std::ostream& os, const Blob& blob)
	{
		os.write(reinterpret_cast<const char*>(blob.data()), blob.size());
		return os;
	}

	friend std::istream& operator>>(std::istream& is, Blob& blob)
	{
		is.read(reinterpret_cast<char*>(blob.data()), blob.size());
		return is;
	}

	friend bool operator<(const Blob& lhs, const Blob& rhs)
	{
		return std::lexicographical_compare(lhs.crbegin(), lhs.crend(), rhs.crbegin(), rhs.crend());
	}
	friend bool operator<=(const Blob& lhs, const Blob& rhs)
	{
		return std::lexicographical_compare(lhs.crbegin(), lhs.crend(), rhs.crbegin(), rhs.crend(), [](auto& l,auto& r){return l <= r;});
	}
	friend bool operator>(const Blob& lhs, const Blob& rhs)
	{
		return std::lexicographical_compare(lhs.crbegin(), lhs.crend(), rhs.crbegin(), rhs.crend(), [](auto& l, auto& r) { return l > r; });
	}
	friend bool operator>=(const Blob& lhs, const Blob& rhs)
	{
		return std::lexicographical_compare(lhs.crbegin(), lhs.crend(), rhs.crbegin(), rhs.crend(), [](auto& l, auto& r) { return l >= r; });
	}
};

namespace std
{
	template<size_t BITS>
	class hash<Blob<BITS>>
	{
		public:
		size_t operator()(const Blob<BITS> &blob) const
		{
			return *reinterpret_cast<const size_t*>(blob.data());
		}
	};
}
