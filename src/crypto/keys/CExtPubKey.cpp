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


// CExtPubKey.cpp

#include <cassert>
#include <secp256k1.h>
#include <lax_der_parsing.h>
#include "CExtPubKey.hpp"
#include "EclipticCurveContext.hpp"

void CExtPubKey::Encode(unsigned char code[BIP32_EXTKEY_SIZE]) const
{
	code[0] = nDepth;
	memcpy(code + 1, vchFingerprint, 4);
	code[5] = (nChild >> 24) & 0xFF;
	code[6] = (nChild >> 16) & 0xFF;
	code[7] = (nChild >> 8) & 0xFF;
	code[8] = (nChild >> 0) & 0xFF;
	memcpy(code + 9, chaincode.begin(), 32);
	assert(pubkey.size() == CPubKey::COMPRESSED_PUBLIC_KEY_SIZE);
	memcpy(code + 41, pubkey.begin(), CPubKey::COMPRESSED_PUBLIC_KEY_SIZE);
}

void CExtPubKey::Decode(const unsigned char code[BIP32_EXTKEY_SIZE])
{
	nDepth = code[0];
	memcpy(vchFingerprint, code + 1, 4);
	nChild = (code[5] << 24) | (code[6] << 16) | (code[7] << 8) | code[8];
	memcpy(chaincode.begin(), code + 9, 32);
	pubkey.Set(code + 41, code + BIP32_EXTKEY_SIZE);
}

bool CExtPubKey::Derive(CExtPubKey& out, unsigned int _nChild) const
{
	out.nDepth = nDepth + 1;
	CKeyID id = pubkey.GetID();
	memcpy(&out.vchFingerprint[0], id.data(), 4);
	out.nChild = _nChild;
	return pubkey.Derive(out.pubkey, out.chaincode, _nChild, chaincode);
}

/* static */ bool CPubKey::CheckLowS(const std::vector<unsigned char>& vchSig)
{
	secp256k1_ecdsa_signature sig;

	auto secp256k1_context_verify = ECCVerifyHandle::secp256k1_context_verify();

	if (!ecdsa_signature_parse_der_lax(secp256k1_context_verify.get(), &sig, vchSig.data(), vchSig.size()))
	{
		return false;
	}
	return (!secp256k1_ecdsa_signature_normalize(ECCVerifyHandle::secp256k1_context_verify().get(), nullptr, &sig));
}
