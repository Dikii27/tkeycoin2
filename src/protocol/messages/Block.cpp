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


// Block.cpp

#include <thread/TaskManager.hpp>
#include "Block.hpp"

REGISTER_MESSAGE(Block)

void protocol::message::Block::Serialize(std::ostream& os) const
{
	if (_block)
	{
		_block->Serialize(os);
	}
}

void protocol::message::Block::Unserialize(std::istream& is)
{
	if (!_block)
	{
		_block = std::make_shared<::Block>();
	}
	_block->Unserialize(is);
}

SVal protocol::message::Block::toSVal() const
{
	SObj obj;
	obj.emplace("COMMAND", command());
	if (_block)
	{
		obj.emplace("payload", _block->toSVal());
	}
	return obj;
}

void protocol::message::Block::apply(
	const std::shared_ptr<Node>& node,
	const std::shared_ptr<Peer>& peer
) const
{
	if (_block->prev().isNull())
	{
		if (Blockchain::addBlock(_block))
		{
			peer->AskHeaders(
				node,
				Blockchain::getBlockLocator(Blockchain::getTopBlockHash()),
				Blockchain::getTopBlockHash()
			);
		}
	}
	else
	{
		TaskManager::enqueue(
			[block = _block]
			{
				Blockchain::addBlock(block);
			}
		);
	}
}
