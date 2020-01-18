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


// GetBlocks.hpp

#pragma once


#include <protocol/types/Message.hpp>

namespace protocol::message
{

class GetBlocks final : public Message
{
	DECLARE_MESSAGE(GetBlocks);

public:
	GetBlocks() = default; // Default-constructor

	GetBlocks(uint32_t version, std::vector<uint256> locator, uint256 stopHash = {})
	: _version(version)
	, _locatorHashes(std::move(locator))
	, _stopHash(std::move(stopHash))
	{
	}

private:
	uint32_t _version = 0;                  // the protocol version
	std::vector<uint256> _locatorHashes;    // block locator object; newest back to genesis block (dense to start, but then sparse)
	uint256 _stopHash;                      // hash of the last desired block header; set to zero to get as many blocks as possible (2000)
};

}



