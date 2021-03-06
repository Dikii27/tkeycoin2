//  Copyright (c) 2018-2019 Tkeycoin Dao
//  Copyright (c) 2020 TKEY DMCC LLC & Tkeycoin Dao. All rights reserved.
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


// Base32.hpp


#pragma once

#include <string>

class Base32 final
{
private:
	static const std::string base32_chars;

public:
	static std::string encode(const void *data, size_t length);

	static inline std::string encode(const std::string& data)
	{
		return encode(data.data(), data.size());
	}

	static std::string decode(std::string const &s);

	static const std::string& charset();
};
