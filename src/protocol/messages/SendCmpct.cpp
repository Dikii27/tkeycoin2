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


// SendCmpct.cpp

#include "SendCmpct.hpp"


REGISTER_MESSAGE(SendCmpct)

void protocol::message::SendCmpct::Serialize(std::ostream& os) const
{
	SerializeList(
		os,
		_announce,
		_version
	);
}

void protocol::message::SendCmpct::Unserialize(std::istream& is)
{
	UnserializeList(
		is,
		_announce,
		_version
	);
}

SVal protocol::message::SendCmpct::toSVal() const
{
	SObj obj;
	obj.emplace("COMMAND", command());
	SObj payload;
	payload.emplace("announce", _announce);
	payload.emplace("version", _version);
	obj.emplace("payload", std::move(payload));
	return obj;
}

void protocol::message::SendCmpct::apply(
	const std::shared_ptr<Node>& node,
	const std::shared_ptr<Peer>& peer
) const
{
	peer->setCompact(_announce, _version);
}
