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


// Version.cpp

#include <protocol/types/Services.hpp>
#include "Version.hpp"

REGISTER_MESSAGE(Version)

void protocol::message::Version::Serialize(std::ostream& os) const
{
	SerializeList(os,
	    _version,       // Identifies protocol version being used by the node
		_services,      // bitfield of features to be enabled for this connection
		_timestamp,      // standard UNIX timestamp in seconds
		_addrRecv,      // The network address of the node receiving this messages
		_addrFrom,      // The network address of the node emitting this messages
		_nonce,         // Node random nonce, randomly generated every time a version messages is sent. This nonce is used to detect connections to self.
		_userAgent,     // User Agent (0x00 if string is 0 bytes long)
		_startHeight,   // The last block received by the emitting node
		_relay           // Whether the remote peer should announce relayed transactions or not, see BIP 0037
	);
}

void protocol::message::Version::Unserialize(std::istream& is)
{
	UnserializeList(is,
	    _version,       // Identifies protocol version being used by the node
		_services,      // bitfield of features to be enabled for this connection
		_timestamp,      // standard UNIX timestamp in seconds
		_addrRecv,      // The network address of the node receiving this messages
		_addrFrom,      // The network address of the node emitting this messages
		_nonce,         // Node random nonce, randomly generated every time a version messages is sent. This nonce is used to detect connections to self.
		_userAgent,     // User Agent (0x00 if string is 0 bytes long)
		_startHeight,   // The last block received by the emitting node
		_relay           // Whether the remote peer should announce relayed transactions or not, see BIP 0037
	);
}

SVal protocol::message::Version::toSVal() const
{
	SObj obj;
	obj.emplace("COMMAND", command());
	SObj payload;
	payload.emplace("version", _version);
	std::string services;
	if (_services & static_cast<uint64_t>(Services::NODE_NETWORK)) services += 'N';
	if (_services & static_cast<uint64_t>(Services::NODE_GETUTXO)) services += 'U';
	if (_services & static_cast<uint64_t>(Services::NODE_BLOOM  )) services += 'B';
	if (_services & static_cast<uint64_t>(Services::NODE_WITNESS)) services += 'W';
	if (_services & static_cast<uint64_t>(Services::NODE_NETWORK_LIMITED)) services += 'L';
	payload.emplace("services", services);
	payload.emplace("timestamp", _timestamp);
	payload.emplace("addrRecv", _addrRecv.toSVal());
	payload.emplace("addrFrom", _addrFrom.toSVal());
	payload.emplace("nonce", _nonce);
	payload.emplace("userAgent", _userAgent.toSVal());
	payload.emplace("relay", _relay != 0);
	obj.emplace("payload", std::move(payload));
	return obj;
}

void protocol::message::Version::apply(
	const std::shared_ptr<Node>& node,
	const std::shared_ptr<Peer>& peer
) const
{
	peer->setVersion(std::min(Node::VERSION, _version));
}
