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


// HttpClient.hpp


#pragma once


#include "../ClientTransport.hpp"
#include "../ServerTransport.hpp"

class HttpClient final : public ClientTransport
{
public:
	HttpClient(const HttpClient&) = delete;
	HttpClient& operator=(const HttpClient&) = delete;
	HttpClient(HttpClient&&) noexcept = delete;
	HttpClient& operator=(HttpClient&&) noexcept = delete;

	HttpClient()
	{
		_log.setName("HttpClient");
		_log.debug("Transport '%s' created", name().c_str());
	}

	~HttpClient() override
	{
		_log.debug("Transport '%s' destroyed", name().c_str());
	}

	bool processing(const std::shared_ptr<Connection>& connection) override;
};
