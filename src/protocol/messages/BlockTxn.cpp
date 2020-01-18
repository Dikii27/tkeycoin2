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


// BlockTxn.cpp

#include "BlockTxn.hpp"

REGISTER_MESSAGE(BlockTxn)

void protocol::message::BlockTxn::Serialize(std::ostream& os) const
{
	_transactions.Serialize(os);
}

void protocol::message::BlockTxn::Unserialize(std::istream& is)
{
	_transactions.Unserialize(is);
}

SVal protocol::message::BlockTxn::toSVal() const
{
	SObj obj;
	obj.emplace("COMMAND", command());
	obj.emplace("payload", _transactions.toSVal());
	return obj;
}

void protocol::message::BlockTxn::apply(
	const std::shared_ptr<Node>& node,
	const std::shared_ptr<Peer>& peer
) const
{
	Blockchain::addTxN(_transactions);
}
