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


// Protocol.hpp

#pragma once


#include <utils/Shareable.hpp>
#include <log/LogHolder.hpp>
#include <utils/Context.hpp>

class Node;

class Protocol final : public Shareable<Protocol>, public LogHolder
{
private:
	std::weak_ptr<Node> _node;

public:
	Protocol() = delete; // Default-constructor
	Protocol(Protocol&&) noexcept = delete; // Move-constructor
	Protocol(const Protocol&) = delete; // Copy-constructor
	~Protocol() override = default; // Destructor
	Protocol& operator=(Protocol&&) noexcept = delete; // Move-assignment
	Protocol& operator=(Protocol const&) = delete; // Copy-assignment

	explicit Protocol(const std::shared_ptr<Node>& node);

	void handler(const std::shared_ptr<Context>& context);
};


