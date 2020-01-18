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


// VariableLengthInteger.cpp

#include "VariableLengthInteger.hpp"

void VariableLengthInteger::Serialize(std::ostream& os) const
{
    if (_value < 253)
    {
        ::Serialize(os, static_cast<uint8_t>(_value));
    }
    else if (_value <= std::numeric_limits<uint16_t>::max())
    {
	    ::Serialize(os, static_cast<uint8_t>(253));
	    ::Serialize(os, static_cast<uint16_t>(_value));
    }
    else if (_value <= std::numeric_limits<uint32_t>::max())
    {
	    ::Serialize(os, static_cast<uint8_t>(254));
	    ::Serialize(os, static_cast<uint32_t>(_value));
    }
    else
    {
	    ::Serialize(os, static_cast<uint8_t>(255));
	    ::Serialize(os, static_cast<uint64_t>(_value));
    }
}

void VariableLengthInteger::Unserialize(std::istream& is)
{
	uint8_t size;
	::Unserialize(is, size);

    if (size < 253)
    {
        _value = size;
    }
    else if (size == 253)
    {
        uint16_t value;
	    ::Unserialize(is, value);
        if (value < 253)
        {
            throw std::ios_base::failure("Non-canonical VariableLengthInteger");
        }
        _value = value;
    }
    else if (size == 254)
    {
	    uint32_t value;
	    ::Unserialize(is, value);
	    if (value <= std::numeric_limits<uint16_t>::max())
	    {
		    throw std::ios_base::failure("Non-canonical VariableLengthInteger");
	    }
	    _value = value;
    }
    else
    {
	    uint64_t value;
	    ::Unserialize(is, value);
	    if (value <= std::numeric_limits<uint32_t>::max())
	    {
		    throw std::ios_base::failure("Non-canonical VariableLengthInteger");
	    }
	    _value = value;
    }
}
