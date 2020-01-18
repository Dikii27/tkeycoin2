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


// DbConnectionPoolFactory.hpp


#pragma once


#include "DbConnectionPool.hpp"
#include "../utils/Dummy.hpp"

class DbConnectionPoolFactory
{
public:
	DbConnectionPoolFactory(const DbConnectionPoolFactory&) = delete;
	DbConnectionPoolFactory& operator=(const DbConnectionPoolFactory&) = delete;
	DbConnectionPoolFactory(DbConnectionPoolFactory&&) noexcept = delete;
	DbConnectionPoolFactory& operator=(DbConnectionPoolFactory&&) noexcept = delete;

private:
	DbConnectionPoolFactory() = default;
	virtual ~DbConnectionPoolFactory()  = default;

	static DbConnectionPoolFactory& getInstance()
	{
		static DbConnectionPoolFactory instance;
		return instance;
	}

	std::map<std::string, std::shared_ptr<DbConnectionPool>(*)(const Setting&)> _creators;

public:
	static Dummy reg(
		const std::string& type,
		std::shared_ptr<DbConnectionPool>(*)(const Setting&)
	) noexcept;
	static std::shared_ptr<DbConnectionPool> create(const Setting& setting);
};
