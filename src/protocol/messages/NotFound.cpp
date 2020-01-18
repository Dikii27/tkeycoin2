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


// NotFound.cpp

#include "NotFound.hpp"
#include <serialization/SerializationWrapper.hpp>
#include <serialization/SArr.hpp>

REGISTER_MESSAGE(NotFound)

void protocol::message::NotFound::Serialize(std::ostream& os) const
{
	::Serialize(os, size_and_(_list));
}

void protocol::message::NotFound::Unserialize(std::istream& is)
{
	::Unserialize(is, size_and_(_list));
}

SVal protocol::message::NotFound::toSVal() const
{
	SObj obj;
	obj.emplace("COMMAND", command());
	SArr payload;
	for (auto& item : _list)
	{
		payload.emplace_back(item.toSVal());
	}
	obj.emplace("payload", std::move(payload));
	return obj;
}

void protocol::message::NotFound::apply(
	const std::shared_ptr<Node>& node,
	const std::shared_ptr<Peer>& peer
) const
{
	peer->ReceiveInventory(node, _list);
}
