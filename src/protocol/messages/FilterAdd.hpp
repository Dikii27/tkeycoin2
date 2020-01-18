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


// FilterAdd.hpp

#pragma once


#include <protocol/types/Message.hpp>

namespace protocol::message
{

class FilterAdd final : public Message
{
public:
	static constexpr char COMMAND[12] = "filteradd";

	FilterAdd() = delete; // Default-constructor
	FilterAdd(FilterAdd&&) noexcept = delete; // Move-constructor
	FilterAdd(const FilterAdd&) = delete; // Copy-constructor
	~FilterAdd() override = default; // Destructor
	FilterAdd& operator=(FilterAdd&&) noexcept = delete; // Move-assignment
	FilterAdd& operator=(FilterAdd const&) = delete; // Copy-assignment

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	const std::string& command() const override
	{
		static const std::string command(COMMAND);
		return command;
	}
};

}
