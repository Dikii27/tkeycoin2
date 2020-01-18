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


// Base58.cpp

#include "Base58.hpp"

/** All alphanumeric characters except for "0", "I", "O", and "l" */
static const char alphabet[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
static const int8_t map[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, -1, -1, -1, -1, -1, -1,
	-1, 9, 10, 11, 12, 13, 14, 15, 16, -1, 17, 18, 19, 20, 21, -1,
	22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, -1, -1, -1, -1, -1,
	-1, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, -1, 44, 45, 46,
	47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

bool Base58::DecodeBase58(std::string_view str, std::vector<uint8_t>& vch)
{
	auto psz = str.cbegin();

	// Skip leading spaces.
	while (*psz && isspace(*psz))
	{
		psz++;
	}

	// Skip and count leading '1's.
	int zeroes = 0;
	int length = 0;
	while (*psz == '1')
	{
		zeroes++;
		psz++;
	}

	// Allocate enough space in big-endian base256 representation.
	size_t size = (str.cend() - psz) * 73322 / 100000 + 1; // log(58) / log(256), rounded up.
	std::vector<uint32_t> b256(size);

	// Process the characters.
	static_assert(sizeof(map) / sizeof(map[0]) == 256, "map.size() should be 256"); // guarantee not out of range

	while (*psz && !isspace(*psz))
	{
		// Decode base58 character
		uint32_t carry = map[(uint8_t) *psz];
		if (carry == -1)  // Invalid b58 character
		{
			return false;
		}
		int i = 0;
		for (auto it = b256.rbegin(); (carry != 0 || i < length) && (it != b256.rend()); ++it, ++i)
		{
			carry += 58 * (*it);
			*it = carry & 0xffu;
			carry >>= 8u;
		}
		assert(carry == 0);
		length = i;
		psz++;
	}

	// Skip trailing spaces.
	while (isspace(*psz))
	{
		psz++;
	}

	// Check of end
	if (*psz != 0)
	{
		return false;
	}

	// Skip leading zeroes in b256.
	auto it = b256.begin() + (size - length);
	while (it != b256.end() && *it == 0)
	{
		it++;
	}

	// Copy result into output vector.
	vch.reserve(zeroes + (b256.end() - it));
	vch.assign(zeroes, 0x00);
	std::copy(it, b256.end(), std::back_inserter(vch));
	return true;
}

template<typename T>
std::string Base58::EncodeBase58(const T begin, const T end)
{
	static_assert(std::is_integral_v<std::decay_t<decltype(*T())>> && sizeof(*T()) == 1, "Except iterators of one-byte integral type");

	T cur = begin;

	// Skip & count leading zeroes.
	size_t zeroes = 0;
	size_t length = 0;
	while (cur != end && *cur == 0)
	{
		cur++;
		zeroes++;
	}

	// Allocate enough space in big-endian base58 representation.
	size_t size = (end - cur) * 136566 / 100000 + 1; // log(256) / log(58), rounded up.
	std::vector<uint8_t> b58(size);

	// Process the bytes.
	while (cur != end)
	{
		uint32_t carry = *cur;
		int i = 0;

		// Apply "b58 = b58 * 256 + ch".
		for (auto it = b58.rbegin(); (carry != 0 || i < length) && (it != b58.rend()); ++it, i++)
		{
			carry += static_cast<uint32_t>(*it) << 8u;
			*it = carry % 58;
			carry /= 58;
		}

		assert(carry == 0);
		length = i;
		cur++;
	}

	// Skip leading zeroes in base58 result.
	auto it = b58.begin() + (size - length);
	while (it != b58.end() && *it == 0)
	{
		it++;
	}

	// Translate the result into a string.
	std::string str;
	str.reserve(zeroes + (b58.end() - it));
	str.assign(zeroes, '1');
	while (it != b58.end())
	{
		str += alphabet[*(it++)];
	}
	return str;
}

template std::string Base58::EncodeBase58(const std::vector<char>::const_iterator begin, const std::vector<char>::const_iterator end);
template std::string Base58::EncodeBase58(const std::vector<uint8_t>::const_iterator begin, const std::vector<uint8_t>::const_iterator end);
template std::string Base58::EncodeBase58(const std::string_view::const_iterator begin, const std::string_view::const_iterator end);
