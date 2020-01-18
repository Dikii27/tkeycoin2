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


// Options.cpp


#include "Options.hpp"

Options::Options(int argc, char **argv)
#ifdef PROJECT_NAME
 #define HELPER4QUOTE(N) #N
: cxxopts::Options(HELPER4QUOTE(PROJECT_NAME), "'" PROJECT_NAME "' project by 'primitive' engine")
 #undef HELPER4QUOTE
#else
	: cxxopts::Options("primitive", "Simple engine for different networking services")
#endif
{
	add_options()
		("c,config", "Path to config file", cxxopts::value<std::string>(_configFile), "[config] helper (?)")
	;

	try
	{
		parse(argc, argv);
	}
	catch (...)
	{
	}
}
