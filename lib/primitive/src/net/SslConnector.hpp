//  Copyright (c) 2017-2019 Tkeycoin Dao. All rights reserved.
//  Copyright (c) 2019-2020 TKEY DMCC LLC & Tkeycoin Dao. All rights reserved.
//  Website: www.tkeycoin.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//


// SslConnector.hpp


#pragma once


#include "TcpConnector.hpp"

#include <openssl/ssl.h>

class SslConnector : public TcpConnector
{
private:
	std::shared_ptr<SSL_CTX> _sslContext;

	std::shared_ptr<TcpConnection> createConnection(const std::shared_ptr<Transport>& transport) override;

public:
	SslConnector() = delete;
	SslConnector(const SslConnector&) = delete;
	SslConnector& operator=(const SslConnector&) = delete;
	SslConnector(SslConnector&& tmp) noexcept = delete;
	SslConnector& operator=(SslConnector&& tmp) noexcept = delete;

	SslConnector(const std::shared_ptr<ClientTransport>& transport, const std::string& host, std::uint16_t port, const std::shared_ptr<SSL_CTX>& sslContext);
	~SslConnector() override = default;

	static std::shared_ptr<Connection> create(const std::shared_ptr<ClientTransport>& transport, const std::string& host, std::uint16_t port, const std::shared_ptr<SSL_CTX>& sslContext)
	{
		return std::make_shared<SslConnector>(transport, host, port, sslContext);
	}
};
