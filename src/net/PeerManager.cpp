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


// PeerManager.cpp

#include "PeerManager.hpp"

std::shared_ptr<Peer> PeerManager::newPeer()
{
	std::shared_ptr<Peer> peer(new Peer(), [](Peer*p){delete p;});

	std::lock_guard <std::mutex> lockGuard(getInstance()._mutexPeers);

	getInstance()._peers.emplace(peer);
	getInstance()._peersById.emplace(peer->id(), peer);

	return peer;
}

std::shared_ptr <Peer> PeerManager::peerById(Peer::Id id)
{
	std::lock_guard <std::mutex> lockGuard(getInstance()._mutexPeersById);
	auto i = getInstance()._peersById.find(id);

	if (i != getInstance()._peersById.end())
	{
		if (auto peer = i->second.lock())
		{
			return peer;
		}
		getInstance()._peersById.erase(i);
	}

	return nullptr;
}

void PeerManager::closePeer(const std::shared_ptr<Peer>& peer)
{
	if (!peer)
	{
		return;
	}

	std::lock_guard <std::mutex> lockGuard(getInstance()._mutexPeers);

	auto i = getInstance()._peers.find(peer);
	if (i == getInstance()._peers.end())
	{
		return;
	}

	getInstance()._peers.erase(i);
}

void PeerManager::forEach(const std::function<void(const std::shared_ptr <Peer>&)>& handler)
{
	std::lock_guard <std::mutex> lockGuard(getInstance()._mutexPeers);

	for (auto& peer : getInstance()._peers)
	{
		handler(peer);
	}
}
