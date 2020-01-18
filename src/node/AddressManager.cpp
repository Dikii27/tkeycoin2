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


// AddressManager.cpp

#include "AddressManager.hpp"

AddressManager AddressManager::_instance;

AddressManager::~AddressManager()
{
	if (!_initialized)
	{
		return;
	}
	bool needSave = false;
	{
		std::lock_guard lockGuard(_mutex);
		needSave = _saveTimer ? _saveTimer->isActive() : false;
	}
	if (needSave)
	{
		save();
	}
}

void AddressManager::init(const Setting& setting)
{
	auto& am = getInstance(false);

	am._log = std::make_unique<Log>("AddressManager");

	am._path = setting.getAs<SStr>("path").value();

	if (setting.has("storageTime"))
	{
		am._storageTime = setting.getAs<SInt>("storageTime").value();
	}

	if (setting.has("addressCapacity"))
	{
		am._addressCapacity = setting.getAs<SInt>("addressCapacity").value();
	}

	if (setting.has("bannedCapacity"))
	{
		am._bannedCapacity = setting.getAs<SInt>("bannedCapacity").value();
	}

	am._initialized = true;

	load();
}

void AddressManager::scheduleSave()
{
	if (!_saveTimer)
	{
		_saveTimer = std::make_shared<Timer>(
			AddressManager::save,
			"Timeout to save addresses"
		);
	}

	_saveTimer->startOnce(std::chrono::seconds(5));
}

bool AddressManager::truncate()
{
	std::lock_guard lockGuard(_mutex);

	uint32_t thresholdTime = time(nullptr) - _storageTime;
	bool changed = false;

	for (const auto list : {&_addresses, &_banned})
	{
		for (auto i = list->begin(); i != list->end();)
		{
			auto ci = i++;
			if (ci->getTime() < thresholdTime)
			{
				list->erase(ci);
				changed = true;
			}
		}
	}

	return changed;
}

void AddressManager::load()
{
	auto& am = getInstance();

	std::vector<protocol::NetworkAddress> address;
	std::vector<protocol::NetworkAddress> banned;

	{
		std::ifstream ifs(am._path, std::ios::binary);

		if (!ifs.is_open())
		{
			if (errno == ENOENT)
			{
				return;
			}
			throw std::runtime_error("Can't open address file '" + am._path + "' for read ← " + strerror(errno));
		}

		::Unserialize(ifs, size_and_(address));
		::Unserialize(ifs, size_and_(banned));
	}

	std::sort(address.rbegin(), address.rend(), protocol::NetworkAddress::ComparatorByTime());
	if (address.size() > am._addressCapacity)
	{
		address.resize(am._addressCapacity);
	}

	{
		std::lock_guard lockGuard(am._mutex);
		am._sorted = address;
		std::move(address.rbegin(), address.rend(), std::inserter(am._addresses, am._addresses.end()));
		std::move(banned.rbegin(), banned.rend(), std::inserter(am._addresses, am._addresses.end()));
	}

	if (am.truncate())
	{
		am.scheduleSave();
	}
}

void AddressManager::save()
{
	auto& am = getInstance();

	am.truncate();

	std::vector<protocol::NetworkAddress> address;
	std::vector<protocol::NetworkAddress> banned;

	{
		std::lock_guard lockGuard(am._mutex);

		address.reserve(am._addresses.size());
		banned.reserve(am._banned.size());

		std::copy(am._addresses.begin(), am._addresses.end(), std::back_inserter(address));
		std::copy(am._banned.begin(), am._banned.end(), std::back_inserter(banned));
	}

	std::sort(address.rbegin(), address.rend(), protocol::NetworkAddress::ComparatorByTime());
	if (address.size() > am._addressCapacity)
	{
		address.resize(am._addressCapacity);
	}

	{
		std::lock_guard lockGuard(am._mutex);
		am._sorted = address;
		std::copy(am._failed.begin(), am._failed.end(), std::back_inserter(address));
	}

	auto path = am._path + "~";

	std::ofstream ofs(path, std::ios::binary | std::ios::trunc);

	if (!ofs.is_open())
	{
		throw std::runtime_error("Can't open address file '" + path + "' for write ← " + strerror(errno));
	}

	::Serialize(ofs, size_and_(address));
	::Serialize(ofs, size_and_(banned));

	ofs.close();

	if (ofs.bad())
	{
		throw std::runtime_error("Error during writting into address file '" + path + "' ← " + strerror(errno));
	}

	if (rename(path.c_str(), am._path.c_str()))
	{
		throw std::runtime_error("Error at rename temporary address file '" + path + "' to  ← '" + am._path + "'" + strerror(errno));
	}
}

void AddressManager::reg(const protocol::NetworkAddress& address)
{
	auto& am = getInstance();

	std::lock_guard lockGuard(am._mutex);

	if (am._banned.find(address) != am._banned.end())
	{
		return;
	}

	{
		auto i = am._failed.find(address);
		if (i != am._addresses.end())
		{
			if (i->getTime() > address.getTime())
			{
				address.setTime(i->getTime());
				am.scheduleSave();
				return;
			}
		}
	}

	{
		auto[i, ok] = am._addresses.emplace(address);
		if (ok)
		{
			am.scheduleSave();
		}
		else if (i->getTime() < address.getTime())
		{
			i->setTime(address.getTime());
			am.scheduleSave();
		}
	}
}

void AddressManager::fail(const protocol::NetworkAddress& address)
{
	auto& am = getInstance();

	bool needSave = false;

	std::lock_guard lockGuard(am._mutex);

	if (am._banned.find(address) != am._banned.end())
	{
		return;
	}

	{
		auto i = am._addresses.find(address);
		if (i != am._addresses.end())
		{
			if (i->getTime() > address.getTime())
			{
				address.setTime(i->getTime());
				needSave = true;
			}
			am._addresses.erase(i);
		}
	}

	{
		auto[i2, ok] = am._failed.emplace(address);
		if (!ok && i2->getTime() < address.getTime())
		{
			i2->setTime(address.getTime());
			needSave = true;
		}
	}

	if (needSave)
	{
		am.scheduleSave();
	}
}

void AddressManager::ban(const protocol::NetworkAddress& address)
{
	auto& am = getInstance();

	std::lock_guard lockGuard(am._mutex);

	am._addresses.erase(address);
	am._failed.erase(address);
	am._banned.emplace(address).first->setTime(time(nullptr));

	am.scheduleSave();
}

std::vector<protocol::NetworkAddress> AddressManager::get(size_t number)
{
	// TODO To implement select address with different age

	auto& am = getInstance();

	std::vector<protocol::NetworkAddress> address;

	{
		std::lock_guard lockGuard(am._mutex);

		number = std::min(am._sorted.size(), number);
		address.reserve(number);

		std::copy(am._sorted.begin(), am._sorted.end(), std::back_inserter(address));
	}

	return address;
}
