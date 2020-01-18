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


// MsgPipe.hpp


#pragma once


#include <transport/Transport.hpp>
#include <net/TcpConnection.hpp>
#include <telemetry/Metric.hpp>
#include <protocol/types/Message.hpp>

class MsgPipe final : public Transport
{
private:
	std::shared_ptr<Handler> _handler;

public:
	MsgPipe(const MsgPipe&) = delete; // Copy-constructor
	MsgPipe& operator=(const MsgPipe&) = delete; // Copy-assignment
	MsgPipe(MsgPipe&&) noexcept = delete; // Move-constructor
	MsgPipe& operator=(MsgPipe&&) noexcept = delete; // Move-assignment

	explicit MsgPipe(const std::shared_ptr<Handler>& handler);

	~MsgPipe() override;

	std::shared_ptr<Metric> metricRequestCount;
	std::shared_ptr<Metric> metricAvgRequestPerSec;
	std::shared_ptr<Metric> metricAvgExecutionTime;

	bool processing(const std::shared_ptr<Connection>& connection) override;
};
