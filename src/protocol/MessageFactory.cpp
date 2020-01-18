//  Copyright (c) 2017-2020 TKEY DMCC LLC & Tkeycoin Dao. All rights reserved.
//  Website: www.tkeycoin.com
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.


// MessageFactory.cpp

#include <iostream>
#include "MessageFactory.hpp"

Dummy protocol::MessageFactory::reg(
	const std::string& name,
	std::shared_ptr<protocol::Message>(* creator)()
) noexcept
{
	auto& factory = getInstance();

	auto i = factory._creators.find(name);
	if (i != factory._creators.end())
	{
		std::cerr << "Internal error: Attempt to register Message with the same name (" << name << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
	factory._creators.emplace(name, creator);
	return Dummy{};
}

std::shared_ptr <protocol::Message> protocol::MessageFactory::create(
	const std::string& name
)
{
	auto& factory = getInstance();

	auto i = factory._creators.find(name);
	if (i == factory._creators.end())
	{
		return nullptr;
	}
	return i->second();
}
