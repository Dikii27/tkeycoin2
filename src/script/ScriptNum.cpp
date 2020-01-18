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


// ScriptNum.cpp

#include <serialization/SerializationWrapper.hpp>
#include "ScriptNum.hpp"

void ScriptNum::Serialize(std::ostream& os) const
{
	std::vector<uint8_t> tmp;

	outer_type value =
		std::max<inner_type>(
			std::numeric_limits<outer_type>::min(),
			std::min<inner_type>(std::numeric_limits<outer_type>::max(), _value)
		);

	const bool negative = value < 0;
	outer_type absValue = negative ? -value : value;

	while (absValue)
	{
		tmp.push_back(absValue & 0xff);
		absValue >>= 8u;
	}

//	- If the most significant byte is >= 0x80 and the value is positive, push a
//	new zero-byte to make the significant byte < 0x80 again.
//
//	- If the most significant byte is >= 0x80 and the value is negative, push a
//	new 0x80 byte that will be popped off when converting to an integral.
//
//	- If the most significant byte is < 0x80 and the value is negative, add
//	0x80 to it, since it will be subtracted and interpreted as a negative when
//	converting to an integral.

	if (tmp.back() & 0x80u)
	{
		tmp.push_back(negative ? 0x80 : 0);
	}
	else if (negative)
	{
		tmp.back() |= 0x80u;
	}

	::Serialize(os, size_and_(tmp));
}

void ScriptNum::Unserialize(std::istream& is)
{
	std::vector<uint8_t> vch;

	::Unserialize(is, size_and_(vch));

	if (vch.empty())
	{
		_value = 0;
		return;
	}

	if (vch.size() > sizeof(outer_type))
	{
		throw std::length_error("script number overflow");
	}

	_value = 0;

	bool negative = vch.back() & 0x80u;
	vch.back() &= ~0x80u;

	for (auto i = vch.rbegin(); i != vch.rend(); ++i)
	{
		_value <<= 8u;
		_value |= *i;
	}

	// If the input vector's most significant byte is 0x80, remove it from
	// the result's msb and return a negative.

	_value = -_value;
}
