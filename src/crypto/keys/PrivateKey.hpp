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


// PrivateKey.hpp

#pragma once

#include <cstdint>
#include <support/allocators/secure.h>
#include <vector>
#include <cassert>
#include <lax_der_privatekey_parsing.h>
#include <support/Random.hpp>
#include "EclipticCurveContext.hpp"

/**
 * secure_allocator is defined in allocators.h
 * CPrivKey is a serialized private key, with all parameters included
 * (PRIVATE_KEY_SIZE bytes)
 */

class PrivateKey final : public std::vector<uint8_t, secure_allocator<uint8_t>>
{
private:
	using base = std::vector<uint8_t, secure_allocator<uint8_t>>;

	/**
	 * secp256k1:
	 */
	static const size_t PRIVATE_KEY_SIZE = 279;
	static const size_t COMPRESSED_PRIVATE_KEY_SIZE = 214;

	/**
	 * see www.keylength.com
	 * script supports up to 75 for single byte push
	 */
	static_assert(
		PRIVATE_KEY_SIZE >= COMPRESSED_PRIVATE_KEY_SIZE,
		"COMPRESSED_PRIVATE_KEY_SIZE is larger than PRIVATE_KEY_SIZE"
	);

	bool _valid;
	bool _compressed;

public:
	PrivateKey(PrivateKey&&) noexcept = delete; // Move-constructor
	PrivateKey(const PrivateKey&) = delete; // Copy-constructor
	~PrivateKey() = default; // Destructor
	PrivateKey& operator=(PrivateKey&&) noexcept = default; // Move-assignment
	PrivateKey& operator=(PrivateKey const&) = delete; // Copy-assignment

	PrivateKey() // Default-constructor
	: _valid(false)
	, _compressed(false)
	{
		// Important: vch must be 32 bytes in length to not break serialization
		base::resize(32);
	}

	template<typename T>
	explicit PrivateKey(const T data)
	: _valid(false)
	, _compressed(false)
	{
		base::assign(data);
	}

	template<typename T>
	explicit PrivateKey(const T begin, const T end)
	: _valid(false)
	, _compressed(false)
	{
		base::assign(begin, end);
	}

	[[nodiscard]]
	PrivateKey clone() const
	{
		assert(_valid);

		std::array<uint8_t, PrivateKey::PRIVATE_KEY_SIZE> keyData{};
		size_t privkeylen = PRIVATE_KEY_SIZE;

		auto secp256k1_context_sign_sp = ECCVerifyHandle::secp256k1_context_sign();
		auto secp256k1_context_sign = secp256k1_context_sign_sp.get();

		int ret = ec_privkey_export_der(secp256k1_context_sign, keyData.data(), &privkeylen, data(), _compressed);
		assert(ret);

		return PrivateKey(keyData.begin(), keyData.end());
	}

	//! Simple read-only vector-like interface.
	[[nodiscard]]
	size_t size() const
	{
		return _valid ? base::size() : 0;
	}

	static bool Check(const uint8_t *ptr)
	{
		auto secp256k1_context_sign_sp = ECCVerifyHandle::secp256k1_context_sign();
		auto secp256k1_context_sign = secp256k1_context_sign_sp.get();

		return secp256k1_ec_seckey_verify(secp256k1_context_sign, ptr);
	}

private:
	void generate(bool compressed)
	{
		do
		{
			Random::GetStrongRandBytes(base::data(), base::size());
		}
		while (!Check(base::data()));
		_valid = true;
		_compressed = compressed;
	}


};

//using CPrivKey = PrivateKey;
