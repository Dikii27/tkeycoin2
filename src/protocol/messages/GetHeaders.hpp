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


// GetHeaders.hpp

#pragma once


#include <protocol/types/Message.hpp>

namespace protocol::message
{
/// Return a headers packet containing the headers of blocks starting right after
/// the last known hash in the block locator object, up to hash_stop or 2000 blocks,
/// whichever comes first. To receive the next block headers, one needs to issue
/// getheaders again with a new block locator object. Keep in mind that some clients
/// may provide headers of blocks which are invalid if the block locator object
/// contains a hash on the invalid branch.
class GetHeaders final : public Message
{
	DECLARE_MESSAGE(GetHeaders);

public:
	GetHeaders() = default; // Default-constructor

	GetHeaders(uint32_t version, std::vector<uint256> locator, uint256 stopHash = {})
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



