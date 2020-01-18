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


// MsgContext.hpp


#pragma once

#include <transport/TransportContext.hpp>
#include <net/Peer.hpp>
#include <protocol/types/Message.hpp>
#include <protocol/messages/Verack.hpp>
#include <protocol/messages/Version.hpp>

class MsgContext final : public TransportContext
{
protected:
	bool _established;
	std::shared_ptr<protocol::message::Version> _localVersionMessage;
	std::shared_ptr<protocol::message::Version> _remoteVersionMessage;
	std::shared_ptr<protocol::Message> _currentMessage;
	std::weak_ptr<Peer> _peer;
	std::function<void(MsgContext&)> _establishHandler;

public:
	MsgContext(const std::shared_ptr<Connection>& connection)
	: TransportContext(connection)
	, _established(false)
	{}
	virtual ~MsgContext() = default;

	void setEstablished();
	bool established()
	{
		return _established;
	}

	std::shared_ptr<protocol::Message> getMessage()
	{
		return _currentMessage;
	}
	void setMessage(const std::shared_ptr<protocol::Message>& message)
	{
		_currentMessage = message;
	}
	void resetMessage()
	{
		_currentMessage.reset();
	}


	std::shared_ptr<protocol::message::Version> getLocalVersion()
	{
		return _localVersionMessage;
	}

	void setLocalVersion(const std::shared_ptr<protocol::message::Version>& version)
	{
		_localVersionMessage = version;
	}

	void resetLocalVersion()
	{
		_localVersionMessage.reset();
	}


	std::shared_ptr<protocol::message::Version> getRemoteVersion()
	{
		return _remoteVersionMessage;
	}

	void setRemoteVersion(const std::shared_ptr<protocol::message::Version>& version)
	{
		_remoteVersionMessage = version;
	}

	void resetRemoteVersion()
	{
		_remoteVersionMessage.reset();
	}


	void assignPeer(const std::shared_ptr<Peer>& peer)
	{
		_peer = peer;
	}
	std::shared_ptr<Peer> getPeer()
	{
		return _peer.lock();
	}
	void resetPeer()
	{
		_peer.reset();
	}


	void setEstablishedHandler(std::function<void(MsgContext&)> handler);


	void transmit(protocol::Message&& msg, bool close = false);
	void transmit(const protocol::Message& msg, bool close = false);
};
