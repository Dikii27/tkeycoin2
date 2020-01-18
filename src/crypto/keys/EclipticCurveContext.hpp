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


// EccVerifyHandle.hpp

#pragma once

#include <support/allocators/secure.h>
#include <secp256k1.h>

class ECCVerifyHandle final
{
public:
	ECCVerifyHandle(ECCVerifyHandle&&) noexcept = delete; // Move-constructor
	ECCVerifyHandle(const ECCVerifyHandle&) = delete; // Copy-constructor
	ECCVerifyHandle& operator=(ECCVerifyHandle&&) noexcept = delete; // Move-assignment
	ECCVerifyHandle& operator=(ECCVerifyHandle const&) = delete; // Copy-assignment

private:
	ECCVerifyHandle(); // Default-constructor
	~ECCVerifyHandle() = default; // Destructor

	static ECCVerifyHandle &getInstance()
	{
		static ECCVerifyHandle instance;
		return instance;
	}

	/* Global secp256k1_context object used for verification. */
	std::shared_ptr<secp256k1_context> _secp256k1_context_verify;

	/* Global secp256k1_context object used for signification. */
	std::shared_ptr<secp256k1_context> _secp256k1_context_sign;

public:
	[[nodiscard]]
	static std::shared_ptr<secp256k1_context> secp256k1_context_verify()
	{
		return getInstance()._secp256k1_context_verify;
	}

	[[nodiscard]]
	static std::shared_ptr<secp256k1_context> secp256k1_context_sign()
	{
		return getInstance()._secp256k1_context_sign;
	}

	static bool InitSanityCheck();
};
