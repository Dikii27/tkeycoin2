//  Copyright (c) 2017-2019 Tkeycoin Dao. All rights reserved.
//  Copyright (c) 2019-2020 TKEY DMCC LLC & Tkeycoin Dao. All rights reserved.
//  Website: www.tkeycoin.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//


// WsClient.hpp


#pragma once


#include "../ClientTransport.hpp"

class WsClient final : public ClientTransport
{
private:
	std::shared_ptr<Transport::Handler> _handler;

public:
	WsClient(const WsClient&) = delete;
	WsClient& operator=(const WsClient&) = delete;
	WsClient(WsClient&&) noexcept = delete;
	WsClient& operator=(WsClient&&) noexcept = delete;

	WsClient(const std::shared_ptr<Handler>& handler)
	: _handler(handler)
	{

		_log.setName("WsClient");
		_log.debug("Transport '%s' created", name().c_str());
	}

	~WsClient() override
	{
		_log.debug("Transport '%s' destroyed", name().c_str());
	}

	bool processing(const std::shared_ptr<Connection>& connection) override;
};
