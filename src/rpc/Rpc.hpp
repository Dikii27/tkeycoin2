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


// Rpc.hpp

#pragma once


#include <utils/Shareable.hpp>
#include <log/LogHolder.hpp>
#include <utils/Context.hpp>

class Node;

class RPC final : public Shareable<RPC>, public LogHolder
{
private:
	std::weak_ptr<Node> _node;

public:
	RPC() = delete; // Default-constructor
	RPC(RPC&&) noexcept = delete; // Move-constructor
	RPC(const RPC&) = delete; // Copy-constructor
	~RPC() override = default; // Destructor
	RPC& operator=(RPC&&) noexcept = delete; // Move-assignment
	RPC& operator=(RPC const&) = delete; // Copy-assignment

	RPC(const std::shared_ptr<Node>& node);

	void handler(const std::shared_ptr<Context>& context);
};
