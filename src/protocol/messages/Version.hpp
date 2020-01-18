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


// Version.hpp

#pragma once


#include <protocol/types/Message.hpp>
#include <protocol/types/NetworkAddress.hpp>
#include <types/VariableLengthString.hpp>
#include <support/Random.hpp>

namespace protocol::message
{
// The version message provides information about the transmitting node
// to the receiving node at the beginning of a connection. Until both
// peers have exchanged version messages, no other messages will be accepted.
class Version final : public Message
{
	DECLARE_MESSAGE(Version);

public:
	Version() // Default-constructor
	{
		_addrRecv.setIgnoreTime();
		_addrFrom.setIgnoreTime();
	}

	Version(
		int32_t version,            // Identifies protocol version being used by the node
		uint64_t services,          // bitfield of features to be enabled for this connection
		NetworkAddress addrRecv,  // The network address of the node receiving this messages
		NetworkAddress addrFrom,  // The network address of the node emitting this messages
		strV&& userAgent,           // User Agent (0x00 if string is 0 bytes long)
		int32_t startHeight,        // The last block received by the emitting node
		uint8_t relay               // Whether the remote peer should announce relayed transactions or not, see BIP 0037
	)
	: _version(version)
	, _services(services)
	, _timestamp(std::time(nullptr))
	, _addrRecv(std::move(addrRecv))
	, _addrFrom(std::move(addrFrom))
	, _nonce(GetRand(UINT64_MAX))
	, _userAgent(userAgent)
	, _startHeight(startHeight)
	, _relay(relay)
	{
		_addrRecv.setIgnoreTime();
		_addrFrom.setIgnoreTime();
	}

private:
	int32_t _version = 0;       // Identifies protocol version being used by the node
	uint64_t _services = 0;		// bitfield of features to be enabled for this connection
	int64_t _timestamp = 0;		// standard UNIX timestamp in seconds
	NetworkAddress _addrRecv;   // The network address of the node receiving this messages
	NetworkAddress _addrFrom;   // The network address of the node emitting this messages
	uint64_t _nonce = 0;        // Node random nonce, randomly generated every time a version messages is sent. This nonce is used to detect connections to self.
    strV _userAgent = "";       // User Agent (0x00 if string is 0 bytes long)
	int32_t _startHeight = 0;   // The last block received by the emitting node
	uint8_t _relay = 0;         // Whether the remote peer should announce relayed transactions or not, see BIP 0037

public:
	static constexpr size_t MIN_MESSAGE_SIZE = sizeof(_version) + sizeof(_services) + sizeof(_timestamp) + \
		NetworkAddress::SIZE + NetworkAddress::SIZE + sizeof(_nonce) + 1 + sizeof(_startHeight) + sizeof(_relay);
	static constexpr size_t MAX_MESSAGE_SIZE = MIN_MESSAGE_SIZE + 1 + 256;

	[[nodiscard]]
	int32_t version() const
	{
		return _version;
	}

	[[nodiscard]]
	uint64_t nonce() const
	{
		return _nonce;
	}
};

}



