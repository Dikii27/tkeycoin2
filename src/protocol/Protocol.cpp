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


// Protocol.cpp

#include "Protocol.hpp"

#include <transport/http/HttpContext.hpp>
#include <transport/Transports.hpp>
#include <transport/messages/MsgContext.hpp>
#include <protocol/messages/SendHeaders.hpp>
#include <transport/messages/OutputConnectionStream.hpp>
#include <cassert>

Protocol::Protocol(const std::shared_ptr<Node>& node)
: LogHolder("Protocol")
, _node(node)
{
	auto transport = Transports::get("protocol");
	if (!transport)
	{
		throw std::runtime_error("Transport 'protocol' not found");
	}
}

void Protocol::handler(const std::shared_ptr<Context>& context)
{
	auto node = _node.lock();
	if (!node)
	{
		return;
	}

	// Приводим тип контекста
	auto msgContext = std::dynamic_pointer_cast<MsgContext>(context);
	assert(msgContext);

	auto peer = std::dynamic_pointer_cast<Peer>(msgContext->getPeer());
	assert(peer);

	std::shared_ptr<protocol::Message> msg;
	try
	{
		msg = msgContext->getMessage();
		assert(msg);

		msg->apply(node, peer);
	}
	catch (const std::exception& exception)
	{
		_log.warn("Exception at processing '" + msg->command() + "' message: " + exception.what());
	}
}
