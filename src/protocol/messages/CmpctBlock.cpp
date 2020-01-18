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


// CmpctBlock.cpp

#include "CmpctBlock.hpp"

REGISTER_MESSAGE(CmpctBlock)

void protocol::message::CmpctBlock::Serialize(std::ostream& os) const
{
	_headerAndShortIDs.Serialize(os);
}

void protocol::message::CmpctBlock::Unserialize(std::istream& is)
{
	_headerAndShortIDs.Unserialize(is);
}

SVal protocol::message::CmpctBlock::toSVal() const
{
	SObj obj;
	obj.emplace("COMMAND", command());
	obj.emplace("payload", std::move(_headerAndShortIDs.toSVal()));
	return obj;
}

void protocol::message::CmpctBlock::apply(
	const std::shared_ptr<Node>& node,
	const std::shared_ptr<Peer>& peer
) const
{
//	peer->AskBlockTxN(node, _headerAndShortIDs); // TODO To implement
}
