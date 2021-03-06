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


// HostnameResolver.hpp


#pragma once

#include <string>
#include <map>
#include <vector>

class HostnameResolver
{
public:
	HostnameResolver(const HostnameResolver&) = delete;
	HostnameResolver& operator=(const HostnameResolver&) = delete;
	HostnameResolver(HostnameResolver&&) noexcept = delete;
	HostnameResolver& operator=(HostnameResolver&&) noexcept = delete;

private:
	HostnameResolver() = default;
	~HostnameResolver() = default;

	std::mutex _mutex;
	std::map<std::string, std::tuple<int, std::vector<in_addr>, time_t>> _cache;

public:
	static HostnameResolver& getInstance()
	{
		static HostnameResolver instance;
		return instance;
	}

	static std::tuple<int, std::vector<in_addr>> resolve(std::string host);
};
