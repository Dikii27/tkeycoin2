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


// AcceptorFactory.cpp


#include "../configs/Setting.hpp"
#include "Acceptor.hpp"
#include "SslAcceptor.hpp"
#include "../utils/SslHelper.hpp"

std::shared_ptr<AcceptorFactory::Creator> AcceptorFactory::creator(const Setting& setting)
{
	std::string host;
	if (!setting.lookupValue("host", host) || host.empty())
	{
		throw std::runtime_error("Bad config or host undefined");
	}

	uint16_t port;
	try
	{
		int port0;
		setting.lookupValue("port", port0);
		if (port0 <= 0 || port0 > 0xFFFF)
		{
			throw std::runtime_error("Bad config: wrong port");
		}
		port = static_cast<uint16_t>(port0);
	}
	catch (const libconfig::SettingNotFoundException& exception)
	{
		throw std::runtime_error("Bad config: port undefined");
	}

	bool secure = false;
	try
	{
		setting.lookupValue("secure", secure);
	}
	catch (const libconfig::SettingNotFoundException& exception)
	{
	}

	if (secure)
	{
		return std::make_shared<Creator>(
			[host = std::move(host), port](const std::shared_ptr<ServerTransport>& transport)
			{
				return SslAcceptor::create(transport, host, port, SslHelper::getServerContext());
			}
		);
	}
	else
	{
		return std::make_shared<Creator>(
			[host = std::move(host), port](const std::shared_ptr<ServerTransport>& transport)
			{
				return TcpAcceptor::create(transport, host, port);
			}
		);
	}
}
