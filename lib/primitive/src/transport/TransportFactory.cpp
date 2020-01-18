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


// TransportFactory.cpp


#include "TransportFactory.hpp"
#include <iostream>

Dummy TransportFactory::reg(const std::string& type, std::shared_ptr<ServerTransport> (* creator)(const Setting&))
{
	auto& factory = getInstance();

	auto i = factory._creators.find(type);
	if (i != factory._creators.end())
	{
		std::cerr << "Internal error: Attempt to register transport with the same type (" << type << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
	factory._creators.emplace(type, creator);
	return Dummy{};
}

std::shared_ptr<ServerTransport> TransportFactory::create(const Setting& setting)
{
	std::string type;
	try
	{
		setting.lookupValue("type", type);
	}
	catch (const libconfig::SettingNotFoundException& exception)
	{
		throw std::runtime_error("Bad config or transport type undefined");
	}

	auto& factory = getInstance();

	auto i = factory._creators.find(type);
	if (i == factory._creators.end())
	{
		throw std::runtime_error("Unknown transport type");
	}
	return i->second(setting);
}
