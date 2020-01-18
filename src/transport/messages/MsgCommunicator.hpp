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


// MsgCommunicator.hpp


#pragma once

#include <sys/ucontext.h>
#include <mutex>
#include <utils/Shareable.hpp>
#include <log/Log.hpp>
#include <transport/messages/MsgClient.hpp>
#include <transport/http/HttpUri.hpp>
#include <net/TcpConnector.hpp>
#include <serialization/Serialization.hpp>
#include <node/Node.hpp>

class MsgCommunicator: public Shareable<MsgCommunicator>
{
private:
	ucontext_t* _savedCtx;
	Log _log;

	HttpUri _uri;
	std::weak_ptr<Node> _node;
	std::shared_ptr<Transport::Handler> _onSuccessHandler;
	std::shared_ptr<std::function<void(MsgCommunicator&)>> _onFailHandler;

	std::shared_ptr<MsgClient> _clientTransport;
	std::string _error;
	std::recursive_mutex _mutex;
	std::shared_ptr<TcpConnector> _connector;
	std::shared_ptr<TcpConnection> _connection;

	enum class State
	{
		INIT = 0,
		CONNECT = 1,
		CONNECTED = 2,
		SUBMIT = 3,
		SUBMITED = 4,
		ESTABLISHED = 5,
		ERROR = 255
	} _state;

	static std::string stateToString(State state)
	{
		switch (state)
		{
			case State::INIT:       { static const std::string s("INIT");       return s; }
			case State::CONNECT:    { static const std::string s("CONNECT");    return s; }
			case State::CONNECTED:  { static const std::string s("CONNECTED");  return s; }
			case State::SUBMIT:     { static const std::string s("SUBMIT");     return s; }
			case State::SUBMITED:   { static const std::string s("SUBMITED");   return s; }
			case State::ESTABLISHED:{ static const std::string s("ESTABLISHED");return s; }
			case State::ERROR:      { static const std::string s("ERROR");      return s; }
			default:                { static const std::string s("UNKNOWN");    return s; }
		}
	}

	void onConnected();
	void failConnect();
	void exceptionAtConnect();

	void submit();
	void exceptionAtSubmit();

	void onComplete();
	void failProcessing();
	void onError();

	void done();

public:
	MsgCommunicator(
		const std::shared_ptr<Node>& node,
		const HttpUri& uri,
		const std::shared_ptr<Transport::Handler>& onSuccessHandler,
		std::shared_ptr<std::function<void(MsgCommunicator&)>>  onFailHandler
	);
	~MsgCommunicator() override;

	void operator()();

	const std::string& uri() const
	{
		return _uri.str();
	}
	std::shared_ptr<TcpConnection> connection() const
	{
		return _connection;
	}
	std::shared_ptr<MsgClient> client() const
	{
		return _clientTransport;
	}
	const std::string& error() const
	{
		return _error;
	}
	bool hasFailed() const
	{
		return !_error.empty();
	}

	void badStep(const std::string& msg);
};
