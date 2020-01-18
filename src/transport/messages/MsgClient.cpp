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


// MsgClient.cpp

#include <cstring>
#include <transport/messages/MsgClient.hpp>
#include <net/TcpConnection.hpp>
#include <transport/messages/MsgContext.hpp>
#include <protocol/messages/Version.hpp>
#include <other/hash.h>
#include <serialization/JsonSerializer.hpp>
#include "InputMemoryStream.hpp"
#include "OutputConnectionStream.hpp"

bool MsgClient::processing(const std::shared_ptr<Connection>& connection_)
{
	auto connection = std::dynamic_pointer_cast<TcpConnection>(connection_);
	if (!connection)
	{
		throw std::runtime_error("Bad connection-type for MsgClient");
	}

	if (!connection->getContext())
	{
		connection->setContext(std::make_shared<MsgContext>(connection));
	}
	auto context = std::dynamic_pointer_cast<MsgContext>(connection->getContext());
	if (!context)
	{
		throw std::runtime_error("Bad context-type for this transport");
	}

	if (context->established())
	{
		throw std::runtime_error("Message communication already established");
	}

	// Counter Version message
	if (!context->getRemoteVersion())
	{
		// Checking the amount of data needed for the message header
		if (connection->dataLen() < protocol::MessageHeader::HEADER_SIZE)
		{
			if (connection->dataLen() > 0)
			{
				_log.debug("Not anough data for read headers (%zu bytes)", connection->dataLen());
			}
			else
			{
				_log.debug("No more data");
			}
			return true;
		}

		// Getting message header
		protocol::MessageHeader msgHeader;
		{
			InputMemoryStream is(connection->dataPtr(), protocol::MessageHeader::HEADER_SIZE);
			msgHeader.Unserialize(is);
		}

		// Checking the magic number
		if (msgHeader.magic() != static_cast<uint32_t>(protocol::Magic::main))
		{
			_log.debug("No match magic number");
			connection->setTtl(std::chrono::milliseconds(50));
			return true;
		}

		// Checking the command
		if (msgHeader.command() != protocol::message::Version::Command())
		{
			_log.debug("Expect version message");
			connection->setTtl(std::chrono::milliseconds(50));
			return true;
		}

		// Checking the value of payload size
		if (msgHeader.length() < protocol::message::Version::MIN_MESSAGE_SIZE ||
			msgHeader.length() > protocol::message::Version::MAX_MESSAGE_SIZE)
		{
			_log.debug("Bad payload length");
			connection->setTtl(std::chrono::milliseconds(50));
			return true;
		}

		// Checking the amount of data needed for the payload
		if (connection->dataLen() < protocol::MessageHeader::HEADER_SIZE + msgHeader.length())
		{
			connection->setTtl(std::chrono::seconds(5));

			_log.debug(
				"Not anough data for read message payload (%zu/%zu bytes)",
				connection->dataLen() - protocol::MessageHeader::HEADER_SIZE, msgHeader.length());
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
			_log.debug("Wrong checksum");
			connection->setTtl(std::chrono::milliseconds(50));
			return true;
		}

		connection->skip(protocol::MessageHeader::HEADER_SIZE);

		// Getting the message
		protocol::message::Version msgVersion;
		{
			InputMemoryStream is(connection->dataPtr(), msgHeader.length());
			msgVersion.Unserialize(is);

			std::ostringstream oss;
			SerializerFactory::create("json", JsonSerializer::PRETTY | JsonSerializer::INDENT)->encode(oss, msgVersion.toSVal());
			_log.info("Recv message '" + msgHeader.command() + "'\n" + oss.str());
		}

		connection->skip(msgHeader.length());

		// Checking the version
		if (msgVersion.version() < Node::MIN_VERSION)
		{
			_log.debug("Unsupported version");
			connection->setTtl(std::chrono::milliseconds(50));
			return true;
		}

		// Checking oneself connection
		if (msgVersion.nonce() == context->getLocalVersion()->nonce())
		{
			_log.debug("Connection to oneself");
			connection->setTtl(std::chrono::milliseconds(50));
			return true;
		}

		context->setRemoteVersion(std::make_shared<protocol::message::Version>(std::move(msgVersion)));

		OutputConnectionStream os(*connection);

		// Sending the Verack message
		{
			protocol::message::Verack msgVerack;
			msgHeader = msgVerack.buildHeader();
			msgHeader.Serialize(os);
			msgVerack.Serialize(os);

			std::ostringstream oss;
			SerializerFactory::create("json", JsonSerializer::PRETTY | JsonSerializer::INDENT)->encode(oss, msgVerack.toSVal());
			_log.info("Send message '" + msgHeader.command() + "'\n" + oss.str());
		}
	}

	// Waiting Verack message

	// Checking the amount of data needed for the message header
	if (connection->dataLen() < protocol::MessageHeader::HEADER_SIZE)
	{
		if (connection->dataLen() > 0)
		{
			_log.debug("Not anough data for read headers (%zu bytes)", connection->dataLen());
		}
		else
		{
			_log.debug("No more data");
		}
		return true;
	}

	// Getting message header
	protocol::MessageHeader msgHeader;
	{
		InputMemoryStream is(connection->dataPtr(), protocol::MessageHeader::HEADER_SIZE);
		msgHeader.Unserialize(is);
	}

	// Checking the magic number
	if (msgHeader.magic() != static_cast<uint32_t>(protocol::Magic::main))
	{
		_log.debug("No match magic number");
		connection->setTtl(std::chrono::milliseconds(50));
		return true;
	}

	// Checking the command
	if (msgHeader.command() != protocol::message::Verack::Command())
	{
		_log.debug("Expect verack message");
		connection->setTtl(std::chrono::milliseconds(50));
		return true;
	}

	// Checking the amount of data needed for the payload
	if (connection->dataLen() < protocol::MessageHeader::HEADER_SIZE + msgHeader.length())
	{
		connection->setTtl(std::chrono::seconds(5));

		_log.debug(
			"Not anough data for read message payload (%zu/%zu bytes)",
			connection->dataLen() - protocol::MessageHeader::HEADER_SIZE, msgHeader.length());
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
		_log.debug("Wrong checksum");
		connection->setTtl(std::chrono::milliseconds(50));
		return true;
	}

	connection->skip(protocol::MessageHeader::HEADER_SIZE);

	// Getting the message
	protocol::message::Verack msgVerack;
	{
		InputMemoryStream is(connection->dataPtr(), msgHeader.length());
		msgVerack.Unserialize(is);

		std::ostringstream oss;
		SerializerFactory::create("json", JsonSerializer::PRETTY | JsonSerializer::INDENT)->encode(oss, msgVerack.toSVal());
		_log.info("Recv message '" + msgHeader.command() + "'\n" + oss.str());
	}

	connection->skip(msgHeader.length());

	// TODO Version negotiation

	// TODO Peer creation here

	context->setHandler(_onSuccessHandler);

	context->setEstablished();

	return true;
}
