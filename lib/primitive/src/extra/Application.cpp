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


// Application.cpp


#include "Application.hpp"

Application::Application(const Setting& setting)
: _isProduction(false)
{
	std::string type;
	if (setting.exists("type"))
	{
		setting.lookupValue("type", type);
	}
	else
	{
		throw std::runtime_error("Undefined type");
	}
	_type = std::move(type);

	std::string appId;
	if (setting.exists("appId"))
	{
		setting.lookupValue("appId", appId);
	}
	else
	{
		throw std::runtime_error("Undefined appId");
	}
	_appId = std::move(appId);

	if (setting.exists("isProduction"))
	{
		setting.lookupValue("isProduction", _isProduction);
	}
}
