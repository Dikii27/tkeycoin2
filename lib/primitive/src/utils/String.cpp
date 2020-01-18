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


// String.cpp


#include <sstream>
#include "String.hpp"

size_t String::utf8strlen(const char *s)
{
	size_t i = 0, j = 0;
	while(s[i])
	{
		if (((uint8_t)(s[i]) & 0xc0) != 0x80) j++;
		i++;
	}
	return j;
}

std::vector<std::string> String::split(const std::string& in, char separator, size_t count)
{
	std::istringstream iss(in);
	std::vector<std::string> param;
	std::string s;
	size_t n = 0;
	while (std::getline(iss, s, (++n < count) ? separator : '\0'))
	{
		param.push_back(std::move(s));
	}
	return param;
}
