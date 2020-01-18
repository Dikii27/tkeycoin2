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


// MsgClient.hpp


#pragma once


#include <transport/ClientTransport.hpp>
#include <utility>

class MsgClient final : public ClientTransport
{
private:
	std::shared_ptr<Transport::Handler> _onSuccessHandler;

public:
	MsgClient(const MsgClient&) = delete;
	MsgClient& operator=(const MsgClient&) = delete;
	MsgClient(MsgClient&&) noexcept = delete;
	MsgClient& operator=(MsgClient&&) noexcept = delete;

	MsgClient(std::shared_ptr<Handler> onSuccessHandler)
	: _onSuccessHandler(std::move(onSuccessHandler))
	{
		_log.setName("MsgClient");
		_log.debug("Transport '%s' created", name().c_str());
	}

	~MsgClient() override
	{
		_log.debug("Transport '%s' destroyed", name().c_str());
	}

	bool processing(const std::shared_ptr<Connection>& connection) override;
};
