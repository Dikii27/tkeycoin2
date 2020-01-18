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


// Base58.hpp

#pragma once

#include <string>
#include <vector>
#include <cassert>
#include <array>
#include <cstring>
#include "hash.h"

class Base58 final
{
public:
	Base58() = delete; // Default-constructor
	Base58(Base58&&) noexcept = delete; // Move-constructor
	Base58(const Base58&) = delete; // Copy-constructor
	~Base58() = delete; // Destructor
	Base58& operator=(Base58&&) noexcept = delete; // Move-assignment
	Base58& operator=(Base58 const&) = delete; // Copy-assignment

	// Encode a byte sequence as a base58-encoded string
	template<typename T>
	static std::string EncodeBase58(const T begin, const T end);

	// Encode various string represantations as a base58-encoded string
	static std::string EncodeBase58(std::string_view data)
	{
		return EncodeBase58(data.begin(), data.end());
	}

	// Encode a byte vector as a base58-encoded string
	template<typename T>
	static std::string EncodeBase58(const std::vector<T>& data)
	{
		static_assert(std::is_integral_v<std::decay_t<T>> && sizeof(T) == 1, "Excepted vector of one-byte integral values");
		return EncodeBase58(data.begin(), data.end());
	}

	// Encode a byte array as a base58-encoded string
	template<typename T, size_t N>
	static std::string EncodeBase58(const std::array<T, N>& data)
	{
		static_assert(std::is_integral_v<std::decay_t<T>> && sizeof(T) == 1, "Excepted array of one-byte integral values");
		return EncodeBase58(data.begin(), data.end());
	}

	// Decode a base58-encoded string (various representations) into a byte vector.
	static bool DecodeBase58(std::string_view str, std::vector<uint8_t>& vch);

	// Encode a byte vector into a base58-encoded string, including checksum
	static std::string EncodeBase58Check(const std::vector<uint8_t>& data)
	{
		std::vector<uint8_t> tmp(data);

		// add 4-byte hash check to the end
		uint256 hash = Hash(tmp.begin(), tmp.end());

		tmp.insert(tmp.end(), hash.begin(), hash.begin() + 4);

		return EncodeBase58(tmp);
	}

	// Decode a base58-encoded string (various representations) that includes a checksum into a byte vector
	// return true if decoding is successful
	static bool DecodeBase58Check(std::string_view str, std::vector<unsigned char>& vchRet)
	{
		auto psz = str.cbegin();

		if (!DecodeBase58(psz, vchRet) || (vchRet.size() < 4))
		{
			vchRet.clear();
			return false;
		}

		// re-calculate the checksum, ensure it matches the included 4-byte checksum
		uint256 hash = Hash(vchRet.begin(), vchRet.end() - 4);

		if (std::memcmp(hash.data(), vchRet.data() + vchRet.size() - 4, 4) != 0)
		{
			vchRet.clear();
			return false;
		}

		vchRet.resize(vchRet.size() - 4);
		return true;
	}
};


