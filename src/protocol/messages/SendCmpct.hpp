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


// SendCmpct.hpp

#pragma once


#include <protocol/types/Message.hpp>

namespace protocol::message
{

class SendCmpct final : public Message
{
DECLARE_MESSAGE(SendCmpct);

public:
	SendCmpct() = default; // Default-constructor

	SendCmpct(bool announce, uint64_t version)
	: _announce(announce ? 1 : 0)
	, _version(version)
	{
	}

private:
	uint8_t _announce = 0;
	uint64_t _version = 0;

public:
	[[nodiscard]]
	uint8_t announce() const
	{
		return _announce;
	}

	[[nodiscard]]
	uint64_t version() const
	{
		return _version;
	}
};

}
