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


// TxWitness.hpp

#pragma once

#include "../serialization/Serialization.hpp"

class TxWitness : public Serializable
{
private:
	// Note that this encodes the data elements being pushed, rather than encoding them as a Script that pushes them.
	std::vector<std::vector<uint8_t>> _stack;

public:
	TxWitness(TxWitness&&) noexcept = default; // Move-constructor
	TxWitness(const TxWitness&) = default; // Copy-constructor
	~TxWitness() = default; // Destructor
	TxWitness& operator=(TxWitness&&) noexcept = delete; // Move-assignment
	TxWitness& operator=(TxWitness const&) = delete; // Copy-assignment

	TxWitness() = default; // Default-constructor

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;


	[[nodiscard]]
	bool IsNull() const
	{
		return _stack.empty();
	}

	void SetNull()
	{
		_stack.clear();
		_stack.shrink_to_fit();
	}

	[[nodiscard]]
	std::string ToString() const
	{
		std::string ret = "ScriptWitness(";
		for (size_t i = 0; i < _stack.size(); i++)
		{
			if (i)
			{
				ret += ", ";
			}
//	        ret += HexStr(stack[i]); // FIXME
		}
		return ret + ")";
	}
};
