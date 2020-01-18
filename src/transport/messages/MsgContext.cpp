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


// MsgContext.cpp


#include <random>
#include <transport/messages/MsgContext.hpp>
#include <utils/encoding/Base64.hpp>
#include <transport/messages/MsgPipe.hpp>
#include <net/ConnectionManager.hpp>
#include <cassert>
#include <serialization/JsonSerializer.hpp>
#include "OutputConnectionStream.hpp"

void MsgContext::setEstablished()
{
	auto connection = _connection.lock();
	if (_established || !connection)
	{
		return;
	}
	_established = true;

	_currentMessage.reset();
	_localVersionMessage.reset();
	_remoteVersionMessage.reset();

	auto transport = std::make_shared<MsgPipe>(_handler);

	auto prevTransport = std::dynamic_pointer_cast<ServerTransport>(connection->transport());
	if (prevTransport)
	{
		transport->metricRequestCount = prevTransport->metricRequestCount;
		transport->metricAvgRequestPerSec = prevTransport->metricAvgRequestPerSec;
		transport->metricAvgExecutionTime = prevTransport->metricAvgExecutionTime;
	}

	connection->setTransport(transport);

	_transmitter =
		std::make_shared<Transport::Transmitter>(
			[transport, wp = _connection]
			(const char* data, size_t size, const std::string& contentType, bool close)
			{
//				transport->transmit(wp.lock(), data, size, contentType, close);
			}
		);

	transport->processing(connection);

	if (_establishHandler)
	{
		_establishHandler(*this);
	}
}

void MsgContext::setEstablishedHandler(std::function<void(MsgContext&)> handler)
{
	_establishHandler = std::move(handler);
}

void MsgContext::transmit(const protocol::Message& msg, bool close)
{
	auto connection = std::dynamic_pointer_cast<TcpConnection>(_connection.lock());
	assert(connection);

	auto context = std::dynamic_pointer_cast<MsgContext>(connection->getContext());
	assert(context);

	if (!context->established())
	{
		throw std::runtime_error("Incomplete messages communication");
	}

	auto header = msg.buildHeader();

	std::ostringstream oss;
	SerializerFactory::create("json", JsonSerializer::PRETTY | JsonSerializer::INDENT)->encode(oss, msg.toSVal());
	Log("~").info("Send message '" + header.command() + "'\n" + oss.str());

	OutputConnectionStream os(*connection);

	header.Serialize(os);
	msg.Serialize(os);

	if (close)
	{
		connection->setTtl(std::chrono::milliseconds(50));
		connection->close();
		connection->resetContext();
	}

	ConnectionManager::watch(connection);
}

void MsgContext::transmit(protocol::Message&& msg, bool close)
{
	transmit(msg, close);
}
