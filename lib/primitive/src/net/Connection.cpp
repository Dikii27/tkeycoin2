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


// Connection.cpp


#include <unistd.h>
#include "Connection.hpp"
#include "ConnectionManager.hpp"

static uint64_t id4noname = 0;

Connection::Connection(const std::shared_ptr<Transport>& transport)
: _log("Connection")
, _transport(transport)
, _sock(-1)
, _timeout(false)
, _closed(true)
, _error(false)
{
	_captured = false;
	_events = 0;
	_postponedEvents = 0;

	_name = "Connection[" + std::to_string(++id4noname) + "]";

	_ready = false;
}

Connection::~Connection()
{
	if (_sock != -1)
	{
		::close(_sock);
	}
}

void Connection::setTtl(std::chrono::milliseconds ttl)
{
	if (!_timeoutForClose)
	{
		_timeoutForClose = std::make_shared<Timer>(
			[wp = std::weak_ptr<std::remove_reference<decltype(*this)>::type>(ptr())](){
				if (auto connection = wp.lock())
				{
					ConnectionManager::timeout(connection);
				}
			},
			"Timeout to close connection"
		);
	}

	_timeoutForClose->restart(ttl);
}
