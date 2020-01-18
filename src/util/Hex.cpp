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


// Hex.cpp

#include "Hex.hpp"

constexpr std::array<uint8_t,256> Hex::_hexMap =
	[]
	() constexpr
	{
		std::array<uint8_t,256> ret{};

		for (size_t i = 0; i < ret.size(); ++i)
		{
			if (i < '0')
			{
				ret[i] = 0xff;
			}
			else if (i <= '9')
			{
				ret[i] = i - '0';
			}
			else if (i < 'A')
			{
				ret[i] = 0xff;
			}
			else if (i <= 'F')
			{
				ret[i] = i - 'A' + 10;
			}
			else if (i < 'a')
			{
				ret[i] = 0xff;
			}
			else if (i <= 'f')
			{
				ret[i] = i - 'a' + 10;
			}
			else
			{
				ret[i] = _noDigit;
			}
		}
		return ret;
	}
	();


std::vector<uint8_t> Hex::Parse(std::string_view str)
{
    std::vector<uint8_t> res;
    bool h = true;
    uint8_t n = 0;
	for (auto chr : str)
	{
		// skip spaces
		if (std::isspace(chr))
		{
			continue;
		}

		uint8_t c = Hex::digitToInt(chr);
		if (c == Hex::_noDigit)
		{
			break;
		}
		if (h)
		{
			n = c << 4u;
		}
		else
		{
			res.push_back(n|c);
			n = 0;
		}
		h = !h;
	}
	return res;
}
