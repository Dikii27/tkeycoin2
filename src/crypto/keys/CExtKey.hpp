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


// CExtKey.hpp

#pragma once

#include <cstring>
#include "common.hpp"
#include "CKey.hpp"
#include "CExtPubKey.hpp"

//class CExtKey final
//{
//public:
//	CExtKey() = delete; // Default-constructor
//	CExtKey(CExtKey&&) noexcept = delete; // Move-constructor
//	CExtKey(const CExtKey&) = delete; // Copy-constructor
//	virtual ~CExtKey() override = default; // Destructor
//	CExtKey& operator=(CExtKey&&) noexcept = delete; // Move-assignment
//	CExtKey& operator=(CExtKey const&) = delete; // Copy-assignment
//
//	CExtKey()
//	{}
//};


struct CExtKey
{
	unsigned char nDepth;
	unsigned char vchFingerprint[4];
	unsigned int nChild;
	ChainCode chaincode;
	CKey key;

	friend bool operator==(const CExtKey& a, const CExtKey& b)
	{
		return a.nDepth == b.nDepth &&
			memcmp(&a.vchFingerprint[0], &b.vchFingerprint[0], sizeof(vchFingerprint)) == 0 &&
			a.nChild == b.nChild &&
			a.chaincode == b.chaincode &&
			a.key == b.key;
	}

	void Encode(unsigned char code[BIP32_EXTKEY_SIZE]) const;

	void Decode(const unsigned char code[BIP32_EXTKEY_SIZE]);

	bool Derive(CExtKey& out, unsigned int nChild) const;

	CExtPubKey Neuter() const;

	void SetSeed(const unsigned char* seed, unsigned int nSeedLen);

	template<typename Stream>
	void Serialize(Stream& s) const
	{
		uintV len = BIP32_EXTKEY_SIZE;
		len.Serialize(s);

//		unsigned int len = BIP32_EXTKEY_SIZE;
//		::WriteCompactSize(s, len);
		unsigned char code[BIP32_EXTKEY_SIZE];
		Encode(code);
		s.write((const char*) &code[0], len);
	}

	template<typename Stream>
	void Unserialize(Stream& s)
	{
		uintV len;
		len.Unserialize(s);
//		unsigned int len = ::ReadCompactSize(s);
		unsigned char code[BIP32_EXTKEY_SIZE];
		if (len != BIP32_EXTKEY_SIZE)
		{
			throw std::runtime_error("Invalid extended key size\n");
		}
		s.read((char*) &code[0], len);
		Decode(code);
	}
};
