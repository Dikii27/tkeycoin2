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


// PeerManager.hpp

#pragma once


#include <cstddef>
#include <memory>
#include <unordered_set>
#include <net/Peer.hpp>

class PeerManager final
{
public:
	PeerManager(PeerManager&&) noexcept = delete; // Move-constructor
	PeerManager(const PeerManager&) = delete; // Copy-constructor
	PeerManager& operator=(PeerManager&&) noexcept = delete; // Move-assignment
	PeerManager& operator=(PeerManager const&) = delete; // Copy-assignment

private:
	PeerManager() = default; // Default-constructor
	~PeerManager() = default; // Destructor

	static PeerManager &getInstance()
	{
		static PeerManager instance;
		return instance;
	}

	struct PeerHash
    {
        size_t operator()(const std::shared_ptr<Peer>& peer) const
        {
            return reinterpret_cast<size_t>(peer.get());
        }
    };

	/// Peers pool
	std::unordered_set<std::shared_ptr<Peer>, PeerHash> _peers;
	std::mutex _mutexPeers;

	/// id => peer
	std::unordered_map<Peer::Id, const std::weak_ptr<Peer>> _peersById;
	std::mutex _mutexPeersById;

public:

	static std::shared_ptr<Peer> newPeer();

	static std::shared_ptr<Peer> peerById(Peer::Id id);

	static void closePeer(const std::shared_ptr<Peer>& peer);

	static void forEach(const std::function<void(const std::shared_ptr<Peer>&)>& handler);
};
