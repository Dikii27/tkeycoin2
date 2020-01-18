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


// MsgPipe.cpp


#include <cstring>
#include <transport/messages/MsgPipe.hpp>
#include <transport/messages/MsgContext.hpp>
#include <net/ConnectionManager.hpp>
#include <other/hash.h>
#include <protocol/MessageFactory.hpp>
#include <serialization/JsonSerializer.hpp>
#include "InputMemoryStream.hpp"

static std::atomic_uint64_t id4noname = 0;

MsgPipe::MsgPipe(const std::shared_ptr<Handler>& handler)
: _handler(handler)
{
	_name = "MsgPipe[" + std::to_string(id4noname.fetch_add(1, std::memory_order_relaxed)) + "]";
	_log.setName("MsgPipe");
	_log.debug("%s created", name().c_str());
}

MsgPipe::~MsgPipe()
{
	_log.debug("%s destroyed", name().c_str());
}

bool MsgPipe::processing(const std::shared_ptr<Connection>& connection_)
{
	auto connection = std::dynamic_pointer_cast<TcpConnection>(connection_);
	if (!connection)
	{
		throw std::runtime_error("Bad connection-type for MsgPipe at processing");
	}

	auto context = std::dynamic_pointer_cast<MsgContext>(connection->getContext());
	if (!context)
	{
		throw std::runtime_error("Bad context-type for this transport");
	}

	if (!context->established())
	{
		throw std::runtime_error("Incomplete messages communication");
	}

	for (;;)
	{
		// Checking the amount of data needed for the message header
		if (connection->dataLen() < protocol::MessageHeader::HEADER_SIZE)
		{
			if (connection->dataLen() > 0)
			{
				_log.debug(
					"Not anough data for read message header (%zu/%zu bytes)",
					connection->dataLen(), protocol::MessageHeader::HEADER_SIZE
				);
				connection->setTtl(std::chrono::seconds(10));
				return true;
			}

			_log.debug("No more data");
			connection->setTtl(std::chrono::seconds(900));
			return true;
		}

		// Getting message header
		protocol::MessageHeader msgHeader;
		{
			InputMemoryStream is(connection->dataPtr(), protocol::MessageHeader::HEADER_SIZE);
			msgHeader.Unserialize(is);
		}

		// Checking the magic number
		if (
			msgHeader.magic() != static_cast<uint32_t>(protocol::Magic::main) &&
			msgHeader.magic() != static_cast<uint32_t>(protocol::Magic::testnet)
		)
		{
			connection->close();
			connection->resetContext();
			connection->setTtl(std::chrono::milliseconds(50));

			_log.info("Bad message: wrong magic number");
			return true;
		}

//		// Checking the value of payload size
//		if (msgHeader.length() > protocol::Message::MAX_PAYLOAD_SIZE)
//		{
//			connection->close();
//			connection->resetContext();
//			connection->setTtl(std::chrono::milliseconds(50));
//			_log.debug("Bad message: bad payload length");
//			return true;
//		}

		// Checking the amount of data needed for the payload
		if (connection->dataLen() < protocol::MessageHeader::HEADER_SIZE + msgHeader.length())
		{
			connection->setTtl(std::chrono::seconds(900));
			_log.debug(
				"Not anough data for read message payload (%zu/%zu bytes)",
				connection->dataLen() - protocol::MessageHeader::HEADER_SIZE, msgHeader.length()
			);
			return true;
		}

		// Checking the checksum
		auto hash = Hash(
			connection->dataPtr() + protocol::MessageHeader::HEADER_SIZE,
			connection->dataPtr() + protocol::MessageHeader::HEADER_SIZE + msgHeader.length()
		);

		uint32_t checksum;
		memcpy(&checksum, connection->dataPtr() + protocol::MessageHeader::CHECKSUM_OFFSET, protocol::MessageHeader::CHECKSUM_SIZE);

		if (msgHeader.checksum() != checksum)
		{
			connection->close();
			connection->resetContext();
			connection->setTtl(std::chrono::milliseconds(50));
			_log.debug("Bad message: Wrong checksum");
			return true;
		}

		connection->skip(protocol::MessageHeader::HEADER_SIZE);

		std::shared_ptr<protocol::Message> msg;
		try
		{
			// Crete message by type
			msg = protocol::MessageFactory::create(msgHeader.command());
			if (msg == nullptr)
			{
				throw std::runtime_error("Unknown message '" + msgHeader.command() + "'");
			}

			InputMemoryStream is(connection->dataPtr(), msgHeader.length());
			msg->Unserialize(is);

			std::ostringstream oss;
			SerializerFactory::create("json", JsonSerializer::PRETTY | JsonSerializer::INDENT)->encode(oss, msg->toSVal());
			_log.info("Recv message '" + msgHeader.command() + "'\n" + oss.str());
		}
		catch(const std::exception& exception)
		{
			connection->close();
			connection->resetContext();
			connection->setTtl(std::chrono::milliseconds(50));
			_log.warn("Bad message: Can't parse message: %s", exception.what());
			return true;
		}

		context->setMessage(msg);

		connection->skip(msgHeader.length());

		if (metricRequestCount) metricRequestCount->addValue();
		if (metricAvgRequestPerSec) metricAvgRequestPerSec->addValue();
		auto beginTime = std::chrono::steady_clock::now();

		context->handle();

		auto now = std::chrono::steady_clock::now();
		auto timeSpent =
			static_cast<double>(std::chrono::steady_clock::duration(now - beginTime).count()) /
			static_cast<double>(std::chrono::steady_clock::duration(std::chrono::seconds(1)).count());
		if (timeSpent > 0)
		{
			if (metricAvgExecutionTime) metricAvgExecutionTime->addValue(timeSpent, now);
		}

		context->resetMessage();
	}
}
