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


// Hex.hpp

#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <iomanip>

class Hex final
{
private:
	static const std::array<uint8_t,256> _hexMap;
	static constexpr uint8_t _noDigit = 0xff;

public:
	Hex() = delete;
	~Hex() = delete;

	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	static constexpr uint8_t digitToInt(const T chr)
	{
		return _hexMap[static_cast<uint8_t>(chr)];
	}

	template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
	static constexpr bool isDigit(const T chr)
	{
		return _hexMap[static_cast<uint8_t>(chr)] != _noDigit;
	}

	static constexpr bool isHexString(std::string_view str)
	{
		for (auto chr : str)
		{
			if (!isDigit(chr))
			{
				return false;
			}
		}
		return (!str.empty()) && (str.size() % 2 == 0);
	}

	static std::vector<uint8_t> Parse(std::string_view str);

	template<typename T, typename std::enable_if_t<std::is_integral_v<T> && sizeof(T) == 1, void>* = nullptr>
	static std::string toString(const T& data)
	{
		std::ostringstream oss;
		oss << std::hex << std::setfill('0');
		for (auto byte : data)
		{
			oss << std::setw(2) << (uint) byte;
		}
		oss << std::dec;
		return oss.str();
	}
};


