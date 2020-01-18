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


// MsgCommunicator.cpp


#include <random>
#include <transport/messages/MsgCommunicator.hpp>
#include <utils/SslHelper.hpp>
#include <net/SslConnector.hpp>
#include <net/ConnectionManager.hpp>
#include <thread/RollbackStackAndRestoreContext.hpp>
#include <transport/messages/MsgContext.hpp>
#include <protocol/messages/Version.hpp>
#include <protocol/MessageFactory.hpp>
#include <cassert>
#include <protocol/messages/SendHeaders.hpp>
#include <protocol/messages/SendCmpct.hpp>
#include <transport/Transports.hpp>
#include <serialization/JsonSerializer.hpp>
#include <utility>
#include "OutputConnectionStream.hpp"


MsgCommunicator::MsgCommunicator(
	const std::shared_ptr<Node>& node,
	const HttpUri& uri,
	const std::shared_ptr<Transport::Handler>& onSuccessHandler,
	std::shared_ptr<std::function<void(MsgCommunicator&)>> onFailHandler
)
: _savedCtx(nullptr)
, _log("MsgCommunicator"/*, Log::Detail::TRACE*/)
, _clientTransport(std::make_shared<MsgClient>(onSuccessHandler))
, _uri(uri)
, _node(node)
, _onSuccessHandler(onSuccessHandler)
, _onFailHandler(std::move(onFailHandler))
, _error("No run")
, _state(State::INIT)
{
	_log.debug("MsgCommunicator for address %s created", _uri.str().c_str());
}

MsgCommunicator::~MsgCommunicator()
{
	std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
	if (_state != State::ERROR && _state != State::ESTABLISHED)
	{
		_log.warn("Destruct on step '" + std::to_string(static_cast<int>(_state)) + "'");
	}
	_log.debug("MsgCommunicator for address %s destroyed", _uri.str().c_str());
}

void MsgCommunicator::operator()()
{
	std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
	if (_state != State::INIT)
	{
		badStep("Step " + std::to_string(static_cast<int>(_state)) + ", but expected " +
			stateToString(State::INIT));
		return;
	}

	if (!Thread::getCurrTaskContext())
	{
		_log.trace("Thread::_currentTaskContextPtrBuffer undefined");
//		throw std::runtime_error("Thread::_currentTaskContextPtrBuffer undefined");
	}
	_savedCtx = Thread::getCurrTaskContext();
	Thread::setCurrTaskContext(nullptr);

	_error.clear();
	_log.trace("--------------------------------------------------------------------------------------------------------");
	_log.trace("Connect...");

	try
	{
		_state = State::CONNECT;

		if (_uri.isSecure())
		{
			auto context = SslHelper::getClientContext();
			_connector = std::make_shared<SslConnector>(_clientTransport, _uri.host(), _uri.port(), context);
		}
		else
		{
			_connector = std::make_shared<TcpConnector>(_clientTransport, _uri.host(), _uri.port());
		}
		_connector->setTtl(std::chrono::seconds(15));

		_connector->addConnectedHandler(
			[wp = std::weak_ptr<MsgCommunicator>(ptr())]
			(const std::shared_ptr<TcpConnection>& connection)
			{
				auto iam = wp.lock();
				if (iam)
				{
					iam->_connector.reset();
					iam->_connection = connection;
					iam->onConnected();
				}
			}
		);

		_connector->addErrorHandler(
			[wp = std::weak_ptr<MsgCommunicator>(ptr())]
			{
				auto iam = wp.lock();
				if (iam)
				{
					iam->_connector.reset();
					iam->failConnect();
				}
			}
		);

		ConnectionManager::add(_connector);
	}
	catch (const std::exception& exception)
	{
		_error = "Exception at connect ← ";
		_error += exception.what();

		exceptionAtConnect();
	}
}

void MsgCommunicator::failConnect()
{
	std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
	if (_state != State::CONNECT)
	{
		badStep("Step " + std::to_string(static_cast<int>(_state)) + " at failConnect(), but expected "
			"CONNECT (" + std::to_string(static_cast<int>(State::CONNECT)) + ")");
		return;
	}

	_log.trace("Fail connect");

	_state = State::ERROR;
	if (_error.empty())
	{
		_error = "Fail connect";
	}

	done();
}

void MsgCommunicator::exceptionAtConnect()
{
	std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
	if (_state != State::CONNECT)
	{
		badStep("Step " + std::to_string(static_cast<int>(_state)) + " at exceptionConnect(), but expected "
			"CONNECT (" + std::to_string(static_cast<int>(State::CONNECT)) + ")");
		return;
	}

	_log.trace("Exception at connect");

	_state = State::ERROR;
	if (_error.empty())
	{
		_error = "Exception at connect";
	}

	done();
}

void MsgCommunicator::onConnected()
{
	std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
	if (_state != State::CONNECT)
	{
		badStep("Step " + std::to_string(static_cast<int>(_state)) + " at onConnected(), but expected " +
			stateToString(State::CONNECT));
		return;
	}

	_log.trace("Connected");

	_state = State::CONNECTED;

	if (!_connection->getContext())
	{
		_connection->setContext(std::make_shared<MsgContext>(_connection));
	}
	auto context = std::dynamic_pointer_cast<MsgContext>(_connection->getContext());
	if (!context)
	{
		throw std::runtime_error("Bad context");
	}

	context->setEstablishedHandler(
		[wp = std::weak_ptr<MsgCommunicator>(ptr())]
			(MsgContext&) {
			auto iam = wp.lock();
			if (iam)
			{
				iam->onComplete();
			}
		}
	);

	_connection->addCompleteHandler(
		[wp = std::weak_ptr<MsgCommunicator>(ptr())]
			(TcpConnection&, const std::shared_ptr<Context>& context_) {
			auto iam = wp.lock();
			if (iam)
			{
				auto context = std::dynamic_pointer_cast<MsgContext>(context_);
				if (context && context->established())
				{
					iam->_log.trace("Disconnected after handshake is completed");
				}
				else
				{
					iam->_log.trace("Disconnected without handshake completion");
					iam->failProcessing();
				}
			}
		}
	);

	_connection->addErrorHandler(
		[wp = std::weak_ptr<MsgCommunicator>(ptr())]
		(TcpConnection&)
		{
			auto iam = wp.lock();
			if (iam)
			{
				iam->failProcessing();
			}
		}
	);

	submit();
}

void MsgCommunicator::submit()
{
	std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
	if (_state != State::CONNECTED)
	{
		badStep("Step " + std::to_string(static_cast<int>(_state)) + " at submit(), but expected " +
			stateToString(State::CONNECTED));
		return;
	}

	_log.trace("Submit...");

	_state = State::SUBMIT;

	try
	{
		auto context = std::dynamic_pointer_cast<MsgContext>(_connection->getContext());
		if (!context)
		{
			throw std::runtime_error("Bad context");
		}

		protocol::NetworkAddress addrRecv(_connection->address());
		protocol::NetworkAddress addrFrom;

		if (auto transport = Transports::get("protocol"))
		{
			addrFrom.set(transport->address());
		}

		auto msg = std::make_shared<protocol::message::Version>(
			Node::VERSION,
			1037, // services, TODO To use real
			std::move(addrRecv),
			std::move(addrFrom),
			Node::USER_AGENT,
			0, // startHeight, TODO To use real
			true // relay  TODO To use real
		);
		if (!msg)
		{
			throw std::runtime_error("Can't create message 'Version'");
		}

		context->setLocalVersion(std::dynamic_pointer_cast<protocol::message::Version>(msg));

		auto header = msg->buildHeader();

		std::ostringstream oss;
		SerializerFactory::create("json", JsonSerializer::PRETTY | JsonSerializer::INDENT)->encode(oss, msg->toSVal());
		_log.info("Send message '" + header.command() + "'\n" + oss.str());

		OutputConnectionStream os(*_connection);

		header.Serialize(os);
		msg->Serialize(os);

		_connection->setTtl(std::chrono::seconds(999));

		_log.trace("Submited");

		_state = State::SUBMITED;

		ConnectionManager::watch(_connection);
	}
	catch (const std::exception& exception)
	{
		_error = "Internal error: Uncatched exception at submit ← ";
		_error += exception.what();

		exceptionAtSubmit();
	}
}

void MsgCommunicator::exceptionAtSubmit()
{
	std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
	if (_state != State::SUBMIT)
	{
		badStep("Step " + std::to_string(static_cast<int>(_state)) + " at exceptionAtSubmit(), but expected " +
			stateToString(State::SUBMIT));
		return;
	}

	_log.trace("Exception at submit");

	_state = State::ERROR;
	if (_error.empty())
	{
		_error = "Exception at submit";
	}

	done();
}

void MsgCommunicator::failProcessing()
{
	std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
	if (
		_state != State::CONNECTED &&
		_state != State::SUBMIT &&
		_state != State::SUBMITED
	)
	{
		badStep("Step " + std::to_string(static_cast<int>(_state)) + " at failProcessing(), but expected " +
			stateToString(State::CONNECTED) + ", " +
			stateToString(State::SUBMIT) + ", " +
			stateToString(State::SUBMITED));
		return;
	}

	_log.trace("Error after connected");

	_state = State::ERROR;
	if (_error.empty())
	{
		_error = "Exception after connected";
	}

	done();
}

void MsgCommunicator::onComplete()
{
	std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
	if (_state == State::ESTABLISHED)
	{
		return;
	}
	if (_state != State::SUBMITED)
	{
		badStep("Step " + std::to_string(static_cast<int>(_state)) + " at onComplete(), but expected " +
			stateToString(State::SUBMITED));
		return;
	}

	auto context = std::dynamic_pointer_cast<MsgContext>(_connection->getContext());
	if (!context)
	{
		throw std::runtime_error("Bad context");
	}

	_connection->setTtl(std::chrono::seconds(900));

	_error.clear();

	_log.trace("Completed");
	_state = State::ESTABLISHED;

	done();
}

void MsgCommunicator::onError()
{
	std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
	if (
		_state != State::CONNECTED &&
		_state != State::SUBMIT &&
		_state != State::SUBMITED
	)
	{
		badStep("Step " + stateToString(_state) + " at onError(), but expected " +
			stateToString(State::CONNECTED) + ", " +
			stateToString(State::SUBMIT) + ", " +
			stateToString(State::SUBMITED));
		return;
	}

	_log.trace("Error at processing");

	_state = State::ERROR;
	if (_error.empty())
	{
		_error = "Error at processing";
	}

	_connection->setTtl(std::chrono::milliseconds(50));

	done();
}

void MsgCommunicator::badStep(const std::string& msg)
{
	std::lock_guard<std::recursive_mutex> lockGuard(_mutex);

	if (_state == State::ERROR || _state == State::ESTABLISHED)
	{
		return;
	}

	_log.warn("Bad step: %s", msg.c_str());

	_state = State::ERROR;
	if (_error.empty())
	{
		_error = "Processing order error";
	}

	done();
}

void MsgCommunicator::done()
{
	ucontext_t* ctx = nullptr;

	if (_error.empty())
	{
		if (!_connection->getContext())
		{
			_connection->setContext(std::make_shared<MsgContext>(_connection));
		}
		auto context = std::dynamic_pointer_cast<MsgContext>(_connection->getContext());
		assert(context);

		_log.trace("Success");
		if (_onSuccessHandler)
		{
			(*_onSuccessHandler)(context);
		}
	}
	else
	{
		_log.trace("Error: " + _error);
		if (_onFailHandler)
		{
			(*_onFailHandler)(*this);
		}
	}

	{
		std::lock_guard<std::recursive_mutex> lockGuard(_mutex);

		_log.trace("Cleanup...");

		if (_state != State::ESTABLISHED)
		{
			_connection.reset();
		}
		_connector.reset();

		ctx = _savedCtx;
		_savedCtx = nullptr;

		_log.trace("Done");
	}

	throw RollbackStackAndRestoreContext(ctx);
}
