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


// CKey.hpp

#pragma once


//class CKey final
//{
//public:
//	CKey() = delete; // Default-constructor
//	CKey(CKey&&) noexcept = delete; // Move-constructor
//	CKey(const CKey&) = delete; // Copy-constructor
//	virtual ~CKey() override = default; // Destructor
//	CKey& operator=(CKey&&) noexcept = delete; // Move-assignment
//	CKey& operator=(CKey const&) = delete; // Copy-assignment
//
//	CKey()
//	{}
//};


#include <vector>
#include <support/allocators/secure.h>
#include <cstring>
#include <types/Blobs.hpp>
#include "common.hpp"
#include "CPubKey.hpp"

/**
 * secure_allocator is defined in allocators.h
 * CPrivKey is a serialized private key, with all parameters included
 * (PRIVATE_KEY_SIZE bytes)
 */
typedef std::vector<uint8_t, secure_allocator<uint8_t>> CPrivKey;

/** An encapsulated private key. */
class CKey
{
public:
	/**
	 * secp256k1:
	 */
	static const unsigned int PRIVATE_KEY_SIZE = 279;
	static const unsigned int COMPRESSED_PRIVATE_KEY_SIZE = 214;
	/**
	 * see www.keylength.com
	 * script supports up to 75 for single byte push
	 */
	static_assert(
		PRIVATE_KEY_SIZE >= COMPRESSED_PRIVATE_KEY_SIZE,
		"COMPRESSED_PRIVATE_KEY_SIZE is larger than PRIVATE_KEY_SIZE"
	);

private:
	//! Whether this private key is valid. We check for correctness when modifying the key
	//! data, so fValid should always correspond to the actual state.
	bool fValid;

	//! Whether the public key corresponding to this private key is (to be) compressed.
	bool fCompressed;

	//! The actual byte data
	std::vector<unsigned char, secure_allocator<unsigned char> > keydata;

	//! Check whether the 32-byte array pointed to by vch is valid keydata.
	bool static Check(const unsigned char* vch);

public:
	//! Construct an invalid private key.
	CKey()
	: fValid(false), fCompressed(false)
	{
		// Important: vch must be 32 bytes in length to not break serialization
		keydata.resize(32);
	}

	friend bool operator==(const CKey& a, const CKey& b)
	{
		return a.fCompressed == b.fCompressed &&
			a.size() == b.size() &&
			memcmp(a.keydata.data(), b.keydata.data(), a.size()) == 0;
	}

	//! Initialize using begin and end iterators to byte data.
	template<typename T>
	void Set(const T pbegin, const T pend, bool fCompressedIn)
	{
//		if (size_t(pend - pbegin) != keydata.size())
//		{
//			fValid = false;
//		}
//		else
			if (Check(&pbegin[0]))
		{
			memcpy(keydata.data(), (unsigned char*) &pbegin[0], keydata.size());
			fValid = true;
			fCompressed = fCompressedIn;
		}
		else
		{
			fValid = false;
		}
	}

	//! Simple read-only vector-like interface.
	unsigned int size() const
	{
		return (fValid ? keydata.size() : 0);
	}

	const unsigned char* begin() const
	{
		return keydata.data();
	}

	const unsigned char* end() const
	{
		return keydata.data() + size();
	}

	//! Check whether this private key is valid.
	bool IsValid() const
	{
		return fValid;
	}

	//! Check whether the public key corresponding to this private key is (to be) compressed.
	bool IsCompressed() const
	{
		return fCompressed;
	}

	//! Generate a new private key using a cryptographic PRNG.
	void MakeNewKey(bool fCompressed);

	/**
	 * Convert the private key to a CPrivKey (serialized OpenSSL private key data).
	 * This is expensive.
	 */
	CPrivKey GetPrivKey() const;

	/**
	 * Compute the public key from a private key.
	 * This is expensive.
	 */
	CPubKey GetPubKey() const;

	/**
	 * Create a DER-serialized signature.
	 * The test_case parameter tweaks the deterministic nonce.
	 */
	bool Sign(const uint256& hash, std::vector<uint8_t>& vchSig, bool grind = true, uint32_t test_case = 0) const;

	/**
	 * Create a compact signature (65 bytes), which allows reconstructing the used public key.
	 * The format is one header byte, followed by two times 32 bytes for the serialized r and s values.
	 * The header byte: 0x1B = first key with even y, 0x1C = first key with odd y,
	 *                  0x1D = second key with even y, 0x1E = second key with odd y,
	 *                  add 0x04 for compressed keys.
	 */
	bool SignCompact(const uint256& hash, std::vector<unsigned char>& vchSig) const;

	//! Derive BIP32 child key.
	bool Derive(CKey& keyChild, ChainCode& ccChild, unsigned int nChild, const ChainCode& cc) const;

	/**
	 * Verify thoroughly whether a private key and a public key match.
	 * This is done using a different mechanism than just regenerating it.
	 */
	bool VerifyPubKey(const CPubKey& vchPubKey) const;

	//! Load private key and check that public key matches.
	bool Load(const CPrivKey& privkey, const CPubKey& vchPubKey, bool fSkipCheck);
};



