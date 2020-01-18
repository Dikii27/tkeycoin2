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


// MemPool.hpp

#pragma once


#include <protocol/types/Message.hpp>

namespace protocol::message
{

class MemPool final : public Message
{
public:
	static constexpr char COMMAND[12] = "mempool";

	MemPool() = delete; // Default-constructor
	MemPool(MemPool&&) noexcept = delete; // Move-constructor
	MemPool(const MemPool&) = delete; // Copy-constructor
	~MemPool() override = default; // Destructor
	MemPool& operator=(MemPool&&) noexcept = delete; // Move-assignment
	MemPool& operator=(MemPool const&) = delete; // Copy-assignment

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



