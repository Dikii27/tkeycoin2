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


// PubKey.hpp

#pragma once

#include <vector>
#include <types/Blobs.hpp>
#include <cstring>
#include <types/VariableLengthInteger.hpp>
#include <other/hash.h>
#include "common.hpp"

class CPubKey final
{
public:
	CPubKey(CPubKey&&) noexcept = default; // Move-constructor
	CPubKey(const CPubKey&) = delete; // Copy-constructor
	~CPubKey() = default; // Destructor
	CPubKey& operator=(CPubKey&&) noexcept = default; // Move-assignment
	CPubKey& operator=(CPubKey const&) = delete; // Copy-assignment

	//! Construct an invalid public key.
	CPubKey() // Default-constructor
	{
		Invalidate();
	}

public:
	/**
	 * secp256k1:
	 */
	static constexpr unsigned int PUBLIC_KEY_SIZE = 65;
	static constexpr unsigned int COMPRESSED_PUBLIC_KEY_SIZE = 33;
	static constexpr unsigned int SIGNATURE_SIZE = 72;
	static constexpr unsigned int COMPACT_SIGNATURE_SIZE = 65;
	/**
	 * see www.keylength.com
	 * script supports up to 75 for single byte push
	 */
	static_assert(
		PUBLIC_KEY_SIZE >= COMPRESSED_PUBLIC_KEY_SIZE,
		"COMPRESSED_PUBLIC_KEY_SIZE is larger than PUBLIC_KEY_SIZE"
	);

private:

	/**
	 * Just store the serialized data.
	 * Its length can very cheaply be computed from the first byte.
	 */
	std::array<uint8_t, PUBLIC_KEY_SIZE> vch;

	//! Compute the length of a pubkey with a given first byte.
	unsigned int static GetLen(unsigned char chHeader)
	{
		if (chHeader == 2 || chHeader == 3)
		{
			return COMPRESSED_PUBLIC_KEY_SIZE;
		}
		if (chHeader == 4 || chHeader == 6 || chHeader == 7)
		{
			return PUBLIC_KEY_SIZE;
		}
		return 0;
	}

	//! Set this key data to be invalid
	void Invalidate()
	{
		vch[0] = 0xFF;
	}

public:

	bool static ValidSize(const std::vector<unsigned char>& vch)
	{
		return vch.size() > 0 && GetLen(vch[0]) == vch.size();
	}


	//! Initialize a public key using begin/end iterators to byte data.
	template<typename T>
	void Set(const T pbegin, const T pend)
	{
		int len = pend == pbegin ? 0 : GetLen(pbegin[0]);
		if (len && len == (pend - pbegin))
		{
			memcpy(vch.data(), (unsigned char*) &pbegin[0], len);
		}
		else
		{
			Invalidate();
		}
	}

	//! Construct a public key using begin/end iterators to byte data.
	template<typename T>
	CPubKey(const T pbegin, const T pend)
	{
		Set(pbegin, pend);
	}

	//! Construct a public key from a byte vector.
	explicit CPubKey(const std::vector<unsigned char>& _vch)
	{
		Set(_vch.begin(), _vch.end());
	}

	//! Simple read-only vector-like interface to the pubkey data.
	[[nodiscard]]
	unsigned int size() const
	{
		return GetLen(vch[0]);
	}

	[[nodiscard]]
	auto data() const
	{
		return vch.data();
	}

	[[nodiscard]]
	auto begin() const
	{
		return vch.cbegin();
	}

	[[nodiscard]]
	auto end() const
	{
		return vch.data() + size();
	}

	const unsigned char& operator[](unsigned int pos) const
	{
		return vch[pos];
	}

	//! Comparator implementation.
	friend bool operator==(const CPubKey& a, const CPubKey& b)
	{
		return a.vch[0] == b.vch[0] &&
			memcmp(a.vch.data(), b.vch.data(), a.size()) == 0;
	}

	friend bool operator!=(const CPubKey& a, const CPubKey& b)
	{
		return !(a == b);
	}

	friend bool operator<(const CPubKey& a, const CPubKey& b)
	{
		return a.vch[0] < b.vch[0] ||
			(a.vch[0] == b.vch[0] && memcmp(a.vch.data(), b.vch.data(), a.size()) < 0);
	}

	//! Implement serialization, as if this was a byte vector.
	template<typename Stream>
	void Serialize(Stream& s) const
	{
		uintV len = size();
		len.Serialize(s);
		s.write((char*) vch.data(), len);
	}

	template<typename Stream>
	void Unserialize(Stream& s)
	{
		uintV len;
		Unserialize(s, len);
		if (len <= PUBLIC_KEY_SIZE)
		{
			s.read((char*) vch.data(), len);
		}
		else
		{
			// invalid pubkey, skip available data
			char dummy;
			while (len--)
			{
				s.read(&dummy, 1);
			}
			Invalidate();
		}
	}

	//! Get the KeyID of this public key (hash of its serialization)
	[[nodiscard]]
	CKeyID GetID() const
	{
		return CKeyID(Hash160(vch.data(), vch.data() + size()));
	}

	//! Get the 256-bit hash of this public key.
	[[nodiscard]]
	uint256 GetHash() const
	{
		return Hash(vch.data(), vch.data() + size());
	}

	/*
	 * Check syntactic correctness.
	 *
	 * Note that this is consensus critical as CheckSig() calls it!
	 */
	[[nodiscard]]
	bool IsValid() const
	{
		return size() > 0;
	}

	//! fully validate whether this is a valid public key (more expensive than IsValid())
	[[nodiscard]]
	bool IsFullyValid() const;

	//! Check whether this is a compressed public key.
	[[nodiscard]]
	bool IsCompressed() const
	{
		return size() == COMPRESSED_PUBLIC_KEY_SIZE;
	}

	/**
	 * Verify a DER signature (~72 bytes).
	 * If this public key is not fully valid, the return value will be false.
	 */
	[[nodiscard]]
	bool Verify(const uint256& hash, const std::vector<unsigned char>& vchSig) const;

	/**
	 * Check whether a signature is normalized (lower-S).
	 */
	static bool CheckLowS(const std::vector<unsigned char>& vchSig);

	//! Recover a public key from a compact signature.
	bool RecoverCompact(const uint256& hash, const std::vector<unsigned char>& vchSig);

	//! Turn this public key into an uncompressed public key.
	bool Decompress();

	//! Derive BIP32 child pubkey.
	bool Derive(CPubKey& pubkeyChild, ChainCode& ccChild, unsigned int nChild, const ChainCode& cc) const;
};


