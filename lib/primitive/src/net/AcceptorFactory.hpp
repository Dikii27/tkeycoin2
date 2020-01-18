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


// AcceptorFactory.hpp


#pragma once

#include <functional>
#include <memory>

#include "../configs/Setting.hpp"

class Connection;
class Acceptor;
class ServerTransport;

class AcceptorFactory final
{
public:
	typedef std::function<std::shared_ptr<Connection>(const std::shared_ptr<ServerTransport>&)> Creator;

	AcceptorFactory(const AcceptorFactory&) = delete;
	AcceptorFactory& operator=(const AcceptorFactory&) = delete;
	AcceptorFactory(AcceptorFactory&&) noexcept = delete;
	AcceptorFactory& operator=(AcceptorFactory&&) noexcept = delete;

private:
	AcceptorFactory() = default;
	~AcceptorFactory() = default;

	static AcceptorFactory& getInstance()
	{
		static AcceptorFactory instance;
		return instance;
	}

public:
	static std::shared_ptr<AcceptorFactory::Creator> creator(const Setting& setting);
};
