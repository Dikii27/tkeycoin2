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


// Amount.hpp

#pragma once


#include <cstdint>
#include <serialization/Serialization.hpp>

class Amount final : public Serializable
{
	using Type = int64_t;
private:
	Type _value;

public:
	Amount(Amount&&) noexcept = default; // Move-constructor
	Amount(const Amount&) = default; // Copy-constructor
	~Amount() = default; // Destructor
	Amount& operator=(Amount&&) noexcept = default; // Move-assignment
	Amount& operator=(Amount const&) = default; // Copy-assignment

	Amount() // Default-constructor
	: _value(0)
	{
	}

	Amount(Type value)
	: _value(value)
	{
	}

	void Serialize(std::ostream& os) const override
	{
		::Serialize(os, _value);
	}

	void Unserialize(std::istream& is) override
	{
		::Unserialize(is, _value);
	}

	[[nodiscard]]
	SVal toSVal() const override
	{
		return _value;
	}
};


