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


// ClientPart.hpp


#pragma once


#include "../../src/configs/Setting.hpp"
#include "../../src/server/Server.hpp"
#include "../../src/serialization/SObj.hpp"
#include "../../src/services/Action.hpp"
#include "../../src/services/ServicePart.hpp"

namespace status
{
class Service;

class ClientPart : public ServicePart
{
private:
	mutable std::mutex _mutex;

	std::string _transportName;
	std::string _uri;

	void handle(const std::shared_ptr<Context>& context);

public:
	explicit ClientPart(const std::shared_ptr<::Service>& service);
	~ClientPart() override
	{
		deinit();
	}

	void init(const Setting& setting) override;
	void deinit() override;
};

}
