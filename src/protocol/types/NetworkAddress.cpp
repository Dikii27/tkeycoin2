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


// NetworkAddress.cpp

#include <serialization/SObj.hpp>
#include "NetworkAddress.hpp"
#include "Services.hpp"


void protocol::NetworkAddress::set(const sockaddr& address)
{
	switch (address.sa_family)
	{
		case AF_INET:
		{
			_ip = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};
			auto ip4 = reinterpret_cast<const sockaddr_in*>(&address)->sin_addr.s_addr;
			memcpy(_ip.data() + 12, &ip4, sizeof(ip4));

			_port = reinterpret_cast<const sockaddr_in*>(&address)->sin_port;
			break;
		}

		case AF_INET6:
		{
			auto ip6 = reinterpret_cast<const sockaddr_in6*>(&address)->sin6_addr.__in6_u.__u6_addr8;
			memcpy(_ip.data(), &ip6, sizeof(ip6));

			_port = reinterpret_cast<const sockaddr_in6*>(&address)->sin6_port;
			break;
		}

		default:
			_ip.fill(0);
			_port = 0;
	}
}

void protocol::NetworkAddress::Serialize(std::ostream& os) const
{
	if (!_ignoreTime)
	{
		::Serialize(os, _time); // Time (version >= 31402). Not present in version messages.
	}
	SerializeList(os,
		_services, // same service(s) listed in version
		_ip,       // IPv6 address. Network byte order.
		_port      // port number, network byte order
	);

}

void protocol::NetworkAddress::Unserialize(std::istream& is)
{
	if (!_ignoreTime)
	{
		::Unserialize(is, _time); // Time (version >= 31402). Not present in version messages.
	}
	UnserializeList(is,
		_services, // same service(s) listed in version
		_ip,       // IPv6 address. Network byte order.
		_port      // port number, network byte order
	);
}

SVal protocol::NetworkAddress::toSVal() const
{
	SObj obj;
	if (!_ignoreTime)
	{
		obj.emplace("time", _time);
	}
	std::string services;
	if (_services & static_cast<uint64_t>(Services::NODE_NETWORK)) services += 'N';
	if (_services & static_cast<uint64_t>(Services::NODE_GETUTXO)) services += 'U';
	if (_services & static_cast<uint64_t>(Services::NODE_BLOOM  )) services += 'B';
	if (_services & static_cast<uint64_t>(Services::NODE_WITNESS)) services += 'W';
	if (_services & static_cast<uint64_t>(Services::NODE_NETWORK_LIMITED)) services += 'L';
	obj.emplace("services", services);

	uint8_t prefix[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};

	char ip[std::max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];

	auto port = htobe16(_port);

	std::string address;

	if (std::equal(_ip.begin(), _ip.begin()+12, prefix))
	{
		in_addr_t ip4;
		memcpy(&ip4, _ip.data() + 12, sizeof(ip4));
		inet_ntop(AF_INET, &ip4, ip, sizeof(ip));
		address = std::string(ip);
		if (port != 20445)
		{
			address += ':' + std::to_string(port);
		}
	}
	else
	{
		in6_addr ip6{};
		memcpy(&ip6, _ip.data(), sizeof(ip6));
		inet_ntop(AF_INET6, &ip6, ip, sizeof(ip));
		if (port != 20445)
		{
			address = '[' + std::string(ip) + "]:" + std::to_string(port);
		}
		else
		{
			address =std::string(ip);
		}
	}

	obj.emplace("address", std::move(address));
	return obj;
}
