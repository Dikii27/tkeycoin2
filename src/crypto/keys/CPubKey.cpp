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


// PubKey.cpp

#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <lax_der_parsing.c>
#include <cassert>
#include <other/Bip32Hash.hpp>
#include "CPubKey.hpp"
#include "EclipticCurveContext.hpp"

bool CPubKey::Verify(const uint256& hash, const std::vector<unsigned char>& vchSig) const
{
	if (!IsValid())
	{
		return false;
	}
	secp256k1_pubkey pubkey;
	secp256k1_ecdsa_signature sig;

	auto secp256k1_context_verify_sp = ECCVerifyHandle::secp256k1_context_verify();
	auto secp256k1_context_verify = secp256k1_context_verify_sp.get();

	if (!secp256k1_ec_pubkey_parse(secp256k1_context_verify, &pubkey, vch.data(), size()))
	{
		return false;
	}
	if (!ecdsa_signature_parse_der_lax(secp256k1_context_verify, &sig, vchSig.data(), vchSig.size()))
	{
		return false;
	}
	/* libsecp256k1's ECDSA verification requires lower-S signatures, which have
	 * not historically been enforced in Bitcoin, so normalize them first. */
	secp256k1_ecdsa_signature_normalize(secp256k1_context_verify, &sig, &sig);
	return secp256k1_ecdsa_verify(secp256k1_context_verify, &sig, hash.begin(), &pubkey);
}

bool CPubKey::RecoverCompact(const uint256& hash, const std::vector<unsigned char>& vchSig)
{
	if (vchSig.size() != COMPACT_SIGNATURE_SIZE)
	{
		return false;
	}
	int recid = (vchSig[0] - 27) & 3;
	bool fComp = ((vchSig[0] - 27) & 4) != 0;

	secp256k1_pubkey pubkey;
	secp256k1_ecdsa_recoverable_signature sig;

	auto secp256k1_context_verify_sp = ECCVerifyHandle::secp256k1_context_verify();
	auto secp256k1_context_verify = secp256k1_context_verify_sp.get();

	if (!secp256k1_ecdsa_recoverable_signature_parse_compact(secp256k1_context_verify, &sig, &vchSig[1], recid))
	{
		return false;
	}
	if (!secp256k1_ecdsa_recover(secp256k1_context_verify, &pubkey, &sig, hash.begin()))
	{
		return false;
	}
	unsigned char pub[PUBLIC_KEY_SIZE];
	size_t publen = PUBLIC_KEY_SIZE;
	secp256k1_ec_pubkey_serialize(
		secp256k1_context_verify, pub, &publen, &pubkey, fComp ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED
	);
	Set(pub, pub + publen);
	return true;
}

bool CPubKey::IsFullyValid() const
{
	if (!IsValid())
	{
		return false;
	}
	secp256k1_pubkey pubkey;

	auto secp256k1_context_verify_sp = ECCVerifyHandle::secp256k1_context_verify();
	auto secp256k1_context_verify = secp256k1_context_verify_sp.get();

	return secp256k1_ec_pubkey_parse(secp256k1_context_verify, &pubkey, vch.data(), size());
}

bool CPubKey::Decompress()
{
	if (!IsValid())
	{
		return false;
	}
	secp256k1_pubkey pubkey;

	auto secp256k1_context_verify_sp = ECCVerifyHandle::secp256k1_context_verify();
	auto secp256k1_context_verify = secp256k1_context_verify_sp.get();

	if (!secp256k1_ec_pubkey_parse(secp256k1_context_verify, &pubkey, vch.data(), size()))
	{
		return false;
	}
	unsigned char pub[PUBLIC_KEY_SIZE];
	size_t publen = PUBLIC_KEY_SIZE;
	secp256k1_ec_pubkey_serialize(secp256k1_context_verify, pub, &publen, &pubkey, SECP256K1_EC_UNCOMPRESSED);
	Set(pub, pub + publen);
	return true;
}

bool CPubKey::Derive(CPubKey& pubkeyChild, ChainCode& ccChild, unsigned int nChild, const ChainCode& cc) const
{
	assert(IsValid());
	assert((nChild >> 31) == 0);
	assert(size() == COMPRESSED_PUBLIC_KEY_SIZE);
	unsigned char out[64];
	BIP32Hash(cc, nChild, *begin(), begin() + 1, out);
	memcpy(ccChild.begin(), out + 32, 32);
	secp256k1_pubkey pubkey;

	auto secp256k1_context_verify_sp = ECCVerifyHandle::secp256k1_context_verify();
	auto secp256k1_context_verify = secp256k1_context_verify_sp.get();

	if (!secp256k1_ec_pubkey_parse(secp256k1_context_verify, &pubkey, vch.data(), size()))
	{
		return false;
	}
	if (!secp256k1_ec_pubkey_tweak_add(secp256k1_context_verify, &pubkey, out))
	{
		return false;
	}
	unsigned char pub[COMPRESSED_PUBLIC_KEY_SIZE];
	size_t publen = COMPRESSED_PUBLIC_KEY_SIZE;
	secp256k1_ec_pubkey_serialize(secp256k1_context_verify, pub, &publen, &pubkey, SECP256K1_EC_COMPRESSED);
	pubkeyChild.Set(pub, pub + publen);
	return true;
}
