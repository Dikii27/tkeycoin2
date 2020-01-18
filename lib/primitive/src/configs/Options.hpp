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


// Options.hpp


#pragma once

#include <memory>
#include <string>
#include "../../lib/cxxopts/include/cxxopts.hpp"

class Options: protected cxxopts::Options
{
private:
	std::string _configFile;

public:
	Options() = delete;
	Options(const Options&) = delete;
	Options& operator=(const Options&) = delete;
	Options(Options&&) noexcept = delete;
	Options& operator=(Options&&) noexcept = delete;

	Options(int argc, char *argv[]);
	~Options() = default;

	const std::string& configFile() const
	{
		return _configFile;
	}
};
