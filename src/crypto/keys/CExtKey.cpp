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


// CExtKey.cpp

#include <crypto/hmac_sha512.h>
#include <cassert>
#include "CExtKey.hpp"

bool CExtKey::Derive(CExtKey& out, unsigned int _nChild) const
{
	out.nDepth = nDepth + 1;
	CKeyID id = key.GetPubKey().GetID();
	memcpy(&out.vchFingerprint[0], id.data(), 4);
	out.nChild = _nChild;
	return key.Derive(out.key, out.chaincode, _nChild, chaincode);
}

void CExtKey::SetSeed(const unsigned char* seed, unsigned int nSeedLen)
{
	static const unsigned char hashkey[] = {'B', 'i', 't', 'c', 'o', 'i', 'n', ' ', 's', 'e', 'e', 'd'};
	std::vector<unsigned char, secure_allocator<unsigned char>> vout(64);
	CHMAC_SHA512(hashkey, sizeof(hashkey)).Write(seed, nSeedLen).Finalize(vout.data());
	key.Set(vout.data(), vout.data() + 32, true);
	memcpy(chaincode.begin(), vout.data() + 32, 32);
	nDepth = 0;
	nChild = 0;
	memset(vchFingerprint, 0, sizeof(vchFingerprint));
}

CExtPubKey CExtKey::Neuter() const
{
	CExtPubKey ret;
	ret.nDepth = nDepth;
	memcpy(&ret.vchFingerprint[0], &vchFingerprint[0], 4);
	ret.nChild = nChild;
	ret.pubkey = key.GetPubKey();
	ret.chaincode = chaincode;
	return std::move(ret);
}

void CExtKey::Encode(unsigned char code[BIP32_EXTKEY_SIZE]) const
{
	code[0] = nDepth;
	memcpy(code + 1, vchFingerprint, 4);
	code[5] = (nChild >> 24) & 0xFF;
	code[6] = (nChild >> 16) & 0xFF;
	code[7] = (nChild >> 8) & 0xFF;
	code[8] = (nChild >> 0) & 0xFF;
	memcpy(code + 9, chaincode.begin(), 32);
	code[41] = 0;
	assert(key.size() == 32);
	memcpy(code + 42, key.begin(), 32);
}

void CExtKey::Decode(const unsigned char code[BIP32_EXTKEY_SIZE])
{
	nDepth = code[0];
	memcpy(vchFingerprint, code + 1, 4);
	nChild = (code[5] << 24) | (code[6] << 16) | (code[7] << 8) | code[8];
	memcpy(chaincode.begin(), code + 9, 32);
	key.Set(code + 42, code + BIP32_EXTKEY_SIZE, true);
}
