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


// SslConnection.hpp


#pragma once

#include "Connection.hpp"

#include "../utils/Buffer.hpp"
#include "ReaderConnection.hpp"
#include "WriterConnection.hpp"
#include "TcpConnection.hpp"

#include <netinet/in.h>
#include <openssl/ssl.h>

class SslConnection : public TcpConnection
{
private:
	std::shared_ptr<SSL_CTX> _sslContext;
	SSL* _sslConnect;

	bool _sslEstablished;
	bool _sslWantRead;
	bool _sslWantWrite;

	bool sslHandshake();

	bool readFromSocket() override;
	bool writeToSocket() override;

public:
	SslConnection() = delete;
	SslConnection(const SslConnection&) = delete;
	SslConnection& operator=(const SslConnection&) = delete;
	SslConnection(SslConnection&& tmp) noexcept = delete;
	SslConnection& operator=(SslConnection&& tmp) noexcept = delete;

	SslConnection(
		const std::shared_ptr<Transport>& transport,
		int fd,
		const sockaddr_in& cliaddr,
		const std::shared_ptr<SSL_CTX>& sslContext,
		bool isOutgoing
	);
	~SslConnection() override;

	void watch(epoll_event& ev) override;

	bool processing() override;
};
