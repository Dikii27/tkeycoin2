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


// AddressManager.hpp

#pragma once


#include <unordered_set>
#include <protocol/types/NetworkAddress.hpp>
#include <mutex>
#include <fstream>
#include <log/Log.hpp>
#include <serialization/SerializationWrapper.hpp>
#include <utils/Timer.hpp>
#include <configs/Setting.hpp>

class AddressManager final
{
public:
	AddressManager(const AddressManager&) = delete;
	AddressManager& operator=(const AddressManager&) = delete;
	AddressManager(AddressManager&&) noexcept = delete;
	AddressManager& operator=(AddressManager&&) noexcept = delete;

private:
	AddressManager() = default;
	~AddressManager();

	static AddressManager _instance;

	static AddressManager& getInstance(bool mustBeInitialized = true)
	{
		if (mustBeInitialized && !_instance._initialized)
		{
			throw std::runtime_error("AddressManager doesn't initialized yet");
		}
		return _instance;
	}

	std::unique_ptr<Log> _log;
	std::shared_ptr<Timer> _saveTimer;
	std::mutex _mutex;

	std::string _path = "addresses.dat";
	size_t _storageTime = 86400 * 30;
	size_t _addressCapacity = 50'000;
	size_t _bannedCapacity = 1'000;

	bool _initialized = false;
	std::unordered_set<protocol::NetworkAddress, protocol::NetworkAddress::Hasher, protocol::NetworkAddress::ComparatorByAddr> _addresses;
	std::unordered_set<protocol::NetworkAddress, protocol::NetworkAddress::Hasher, protocol::NetworkAddress::ComparatorByAddr> _banned;
	std::unordered_set<protocol::NetworkAddress, protocol::NetworkAddress::Hasher, protocol::NetworkAddress::ComparatorByAddr> _failed;
	std::vector<protocol::NetworkAddress> _sorted;

	void scheduleSave();
	bool truncate();

	static void load();
	static void save();

public:
	static void init(const Setting& configs);

	static std::vector<protocol::NetworkAddress> get(size_t number);

	static void reg(const protocol::NetworkAddress& address);

	static void fail(const protocol::NetworkAddress& address);

	static void ban(const protocol::NetworkAddress& address);

	static bool isBanned(const protocol::NetworkAddress& address)
	{
		auto& am = getInstance();
		std::lock_guard lockGuard(am._mutex);
		return am._banned.find(address) != am._banned.end();
	}

	static size_t registeredCount()
	{
		auto& am = getInstance();
		std::lock_guard lockGuard(am._mutex);
		return am._addresses.size();
	}
	static size_t bannedCount()
	{
		auto& am = getInstance();
		std::lock_guard lockGuard(am._mutex);
		return am._banned.size();
	}
};


