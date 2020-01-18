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


// NetworkAddress.hpp

#pragma once


#include <cstdint>
#include <cstddef>
#include <serialization/Serialization.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <other/MurmurHash2.hpp>

namespace protocol
{

class NetworkAddress final : public Serializable
{
private:
	mutable uint32_t _time = 0;         // Time (version >= 31402). Not present in version messages.
	uint64_t _services = 0;             // same service(s) listed in version
	std::array<uint8_t,16> _ip = {};    // IPv6 address. Network byte order.
										//  The original client only supported IPv4 and only read the last 4 bytes to get the IPv4 address.
										//  However, the IPv4 address is written into the messages as a 16 byte IPv4-mapped IPv6 address
										//	(12 bytes 00 00 00 00 00 00 00 00 00 00 FF FF, followed by the 4 bytes of the IPv4 address).
	uint16_t _port = 0;                 // port number, network byte order
	mutable bool _ignoreTime = false;

public:
	static constexpr size_t SIZE = sizeof(_time) + sizeof(_services) + sizeof(_ip) + sizeof(_port);

	NetworkAddress() = default; // Default-constructor
	NetworkAddress(NetworkAddress&&) noexcept = default; // Move-constructor
	NetworkAddress(const NetworkAddress&) = default; // Copy-constructor
	~NetworkAddress() = default; // Destructor
	NetworkAddress& operator=(NetworkAddress&&) noexcept = default; // Move-assignment
	NetworkAddress& operator=(NetworkAddress const&) = default; // Copy-assignment

	explicit NetworkAddress(const sockaddr& address)
	: _time(0)
	, _services(0)
	, _ignoreTime(false)
	{
		set(address);
	}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;

	void set(const sockaddr& address);
	void get(sockaddr& address);

	void setIgnoreTime()
	{
		_ignoreTime = true;
		_time = 0;
	}

	void setTime(uint32_t time) const
	{
		_ignoreTime = false;
		_time = time;
	}
	[[nodiscard]]
	uint32_t getTime() const
	{
		return _ignoreTime ? 0 : _time;
	}

	struct Hasher
	{
		public:
		size_t operator()(const NetworkAddress &blob) const
		{
			return MurmurHash64B(blob._ip.data(), blob._ip.size(), 0);
		}
	};

	struct ComparatorByAddr
	{
		public:
		size_t operator()(const NetworkAddress &lhs, const NetworkAddress &rhs) const
		{
			return lhs._ip == rhs._ip && lhs._port == rhs._port;
		}
	};

	struct ComparatorByTime
	{
		public:
		size_t operator()(const NetworkAddress &lhs, const NetworkAddress &rhs) const
		{
			return lhs._time < rhs._time;
		}
	};
};

}
