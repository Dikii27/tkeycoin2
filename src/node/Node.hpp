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


// Node.hpp

#pragma once


#include <configs/Setting.hpp>
#include <utils/Shareable.hpp>
#include <rpc/Rpc.hpp>
#include <node/Blockchain.hpp>
#include <protocol/Protocol.hpp>
#include <protocol/types/NetworkAddress.hpp>
#include <net/Peer.hpp>
#include <unordered_set>
#include <blockchain/Transaction.hpp>
#include <blockchain/Block.hpp>

class MsgCommunicator;

class Node final : public Shareable<Node>
{
public:
	static constexpr int32_t VERSION = 8'00'00;
	static constexpr int32_t MIN_VERSION = 8'00'00;
	static constexpr char const * USER_AGENT = "/TKeyCoin-Protocol:8.0.0/TKeyCoin-Server:2.0.0/"; // TODO Set by config
	static constexpr size_t MAX_INV_COUNT = 50'000;

private:
	std::shared_ptr<Blockchain> _blockchain;

	std::unordered_set<std::shared_ptr<MsgCommunicator>> _peers;

	std::shared_ptr<Protocol> _protocol;
	std::shared_ptr<RPC> _rpc;

public:
	Node() = delete; // Default-constructor
	Node(Node&&) noexcept = delete; // Move-constructor
	Node(const Node&) = delete; // Copy-constructor
	~Node() override = default; // Destructor
	Node& operator=(Node&&) noexcept = delete; // Move-assignment
	Node& operator=(Node const&) = delete; // Copy-assignment

	explicit Node(const Setting& setting);

	std::shared_ptr<Protocol> protocol()
	{
		return _protocol;
	}
	std::shared_ptr<RPC> rpc()
	{
		return _rpc;
	}

	void up();
	void down();

	void connectToPeers();

	void announceTx(const std::shared_ptr<Transaction>& tx) const;
	void announceBlock(const std::shared_ptr<Block>& block) const;
};


