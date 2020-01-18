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


// MsgServer.cpp


#include <sstream>
#include <memory>
#include <cstring>
#include <net/ConnectionManager.hpp>
#include <net/TcpConnection.hpp>
#include <server/Server.hpp>
#include <utils/encoding/Base64.hpp>
#include <utils/hash/SHA1.hpp>
#include <transport/messages/MsgContext.hpp>
#include <transport/messages/MsgServer.hpp>
#include <utils/String.hpp>
#include <protocol/types/Message.hpp>
#include <transport/http/HttpResponse.hpp>
#include <protocol/messages/Version.hpp>
#include <other/hash.h>
#include <protocol/messages/SendHeaders.hpp>
#include "InputMemoryStream.hpp"

REGISTER_TRANSPORT(messages, MsgServer);

bool MsgServer::processing(const std::shared_ptr<Connection>& connection_)
{
	auto connection = std::dynamic_pointer_cast<TcpConnection>(connection_);
	if (!connection)
	{
		throw std::runtime_error("Bad connection-type for this transport");
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
		connection->setTtl(std::chrono::seconds(5));

		_log.debug("Not anough data for read message header (%zu/%zu bytes)", connection->dataLen(), protocol::MessageHeader::HEADER_SIZE);
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
		connection->resetContext();
		connection->setTtl(std::chrono::milliseconds(50));

		_log.info("Bad message: wrong magic number");
		return true;
	}

	// Checking the command
	if (msgHeader.command() != protocol::message::Version::Command())
	{
		connection->resetContext();
		connection->setTtl(std::chrono::milliseconds(50));

		_log.info("Bad message: expect version message");
		return true;
	}

	// Checking the value of payload size
	if (msgHeader.length() < protocol::message::Version::MIN_MESSAGE_SIZE ||
		msgHeader.length() > protocol::message::Version::MAX_MESSAGE_SIZE)
	{
		_log.debug("Bad message: bad payload length");
		connection->setTtl(std::chrono::milliseconds(50));
		return true;
	}

	// Checking the amount of data needed for the payload
	if (connection->dataLen() < protocol::MessageHeader::HEADER_SIZE + msgHeader.length())
	{
		connection->setTtl(std::chrono::seconds(5));

		_log.debug("Not anough data for read message payload (%zu/%zu bytes)",
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
		_log.debug("Bad message: Wrong checksum");
		connection->setTtl(std::chrono::milliseconds(50));
		return true;
	}

	connection->skip(protocol::MessageHeader::HEADER_SIZE);

	// Getting the message
	protocol::message::Version msgVersion;
	{
		InputMemoryStream is(connection->dataPtr(), msgHeader.length());
		msgVersion.Unserialize(is);
	}

	auto handler = getHandler("/");
	if (!handler)
	{
		connection->resetContext();
		connection->setTtl(std::chrono::milliseconds(50));

		_log.info("Internal error: handler wasn't defined");
		return true;
	}

	context->setHandler(handler);

	context->setEstablished();

	return true;
}

void MsgServer::bindHandler(const std::string& selector, const std::shared_ptr<ServerTransport::Handler>& handler)
{
	std::lock_guard<std::mutex> lockGuard(_mutex);

	if (_handlers.find(selector) != _handlers.end())
	{
		throw std::runtime_error("Handler already set early");
	}

	_handlers.emplace(selector, handler);
}

void MsgServer::unbindHandler(const std::string& selector)
{
	std::lock_guard<std::mutex> lockGuard(_mutex);

	_handlers.erase(selector);
}

std::shared_ptr<ServerTransport::Handler> MsgServer::getHandler(const std::string& subject)
{
	auto i = _handlers.find(subject);
	if (i != _handlers.end())
	{
		return i->second;
	}

	return nullptr;
}
