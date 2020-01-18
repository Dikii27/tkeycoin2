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


// SInt.hpp


#pragma once


#include "SNum.hpp"

#include <sstream>

class SInt final: public SNum
{
public:
	typedef int64_t type;

private:
	type _value;

public:
	SInt(): _value(0) {};

	SInt(type value)
	: _value(value)
	{
	}

	type value() const
	{
		return _value;
	}

	template<typename T, typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, void>::type* = nullptr>
	operator T() const
	{
		return _value;
	}

	explicit operator std::string() const
	{
		return std::to_string(_value);
	}
};
