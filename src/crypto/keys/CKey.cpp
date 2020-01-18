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


// CKey.cpp

#include <cassert>
#include <secp256k1.h>
#include <lax_der_privatekey_parsing.c>
#include <support/Random.hpp>
#include <secp256k1_recovery.h>
#include <util/Endians.hpp>
#include <other/Bip32Hash.hpp>
#include "CKey.hpp"
#include "EclipticCurveContext.hpp"

bool CKey::Check(const unsigned char* vch)
{
	auto secp256k1_context_sign_sp = ECCVerifyHandle::secp256k1_context_sign();
	auto secp256k1_context_sign = secp256k1_context_sign_sp.get();

	return secp256k1_ec_seckey_verify(secp256k1_context_sign, vch);
}

void CKey::MakeNewKey(bool fCompressedIn)
{
	do
	{
		Random::GetStrongRandBytes(keydata.data(), keydata.size());
	}
	while (!Check(keydata.data()));
	fValid = true;
	fCompressed = fCompressedIn;
}

CPrivKey CKey::GetPrivKey() const
{
	assert(fValid);
	CPrivKey privkey;
	int ret;
	size_t privkeylen;
	privkey.resize(PRIVATE_KEY_SIZE);
	privkeylen = PRIVATE_KEY_SIZE;

	auto secp256k1_context_sign_sp = ECCVerifyHandle::secp256k1_context_sign();
	auto secp256k1_context_sign = secp256k1_context_sign_sp.get();

	ret = ec_privkey_export_der(secp256k1_context_sign, privkey.data(), &privkeylen, begin(), fCompressed);
	assert(ret);
	privkey.resize(privkeylen);
	return privkey;
}

CPubKey CKey::GetPubKey() const
{
	assert(fValid);
	secp256k1_pubkey pubkey;
	size_t clen = CPubKey::PUBLIC_KEY_SIZE;
	CPubKey result;

	auto secp256k1_context_sign_sp = ECCVerifyHandle::secp256k1_context_sign();
	auto secp256k1_context_sign = secp256k1_context_sign_sp.get();

	int ret = secp256k1_ec_pubkey_create(secp256k1_context_sign, &pubkey, begin());
	assert(ret);
	secp256k1_ec_pubkey_serialize(
		secp256k1_context_sign, (unsigned char*) result.data(), &clen, &pubkey,
		fCompressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED);
	assert(result.size() == clen);
	assert(result.IsValid());
	return result;
}

// Check that the sig has a low R value and will be less than 71 bytes
bool SigHasLowR(const secp256k1_ecdsa_signature* sig)
{
	unsigned char compact_sig[64];

	auto secp256k1_context_sign_sp = ECCVerifyHandle::secp256k1_context_sign();
	auto secp256k1_context_sign = secp256k1_context_sign_sp.get();

	secp256k1_ecdsa_signature_serialize_compact(secp256k1_context_sign, compact_sig, sig);

	// In DER serialization, all values are interpreted as big-endian, signed integers. The highest bit in the integer indicates
	// its signed-ness; 0 is positive, 1 is negative. When the value is interpreted as a negative integer, it must be converted
	// to a positive value by prepending a 0x00 byte so that the highest bit is 0. We can avoid this prepending by ensuring that
	// our highest bit is always 0, and thus we must check that the first byte is less than 0x80.
	return compact_sig[0] < 0x80;
}

bool CKey::Sign(const uint256& hash, std::vector<uint8_t>& vchSig, bool grind, uint32_t test_case) const
{
	if (!fValid)
	{
		return false;
	}
	vchSig.resize(CPubKey::SIGNATURE_SIZE);
	size_t nSigLen = CPubKey::SIGNATURE_SIZE;
	unsigned char extra_entropy[32] = {0};
	WriteLE32(extra_entropy, test_case);
	secp256k1_ecdsa_signature sig;
	uint32_t counter = 0;

	auto secp256k1_context_sign_sp = ECCVerifyHandle::secp256k1_context_sign();
	auto secp256k1_context_sign = secp256k1_context_sign_sp.get();

	int ret = secp256k1_ecdsa_sign(
		secp256k1_context_sign, &sig, hash.begin(), begin(), secp256k1_nonce_function_rfc6979,
		(!grind && test_case) ? extra_entropy : nullptr
	);

	// Grind for low R
	while (ret && !SigHasLowR(&sig) && grind)
	{
		WriteLE32(extra_entropy, ++counter);
		ret = secp256k1_ecdsa_sign(secp256k1_context_sign, &sig, hash.begin(), begin(), secp256k1_nonce_function_rfc6979, extra_entropy);
	}
	assert(ret);
	secp256k1_ecdsa_signature_serialize_der(secp256k1_context_sign, vchSig.data(), &nSigLen, &sig);
	vchSig.resize(nSigLen);
	return true;
}

bool CKey::VerifyPubKey(const CPubKey& pubkey) const
{
	if (pubkey.IsCompressed() != fCompressed)
	{
		return false;
	}
	unsigned char rnd[8];
	std::string str = "Bitcoin key verification\n";
	GetRandBytes(rnd, sizeof(rnd));
	uint256 hash;
	CHash256().Write((unsigned char*) str.data(), str.size()).Write(rnd, sizeof(rnd)).Finalize(hash.begin());
	std::vector<unsigned char> vchSig;
	Sign(hash, vchSig);
	return pubkey.Verify(hash, vchSig);
}

bool CKey::SignCompact(const uint256& hash, std::vector<unsigned char>& vchSig) const
{
	if (!fValid)
	{
		return false;
	}
	vchSig.resize(CPubKey::COMPACT_SIGNATURE_SIZE);
	int rec = -1;
	secp256k1_ecdsa_recoverable_signature sig;

	auto secp256k1_context_sign_sp = ECCVerifyHandle::secp256k1_context_sign();
	auto secp256k1_context_sign = secp256k1_context_sign_sp.get();

	int ret = secp256k1_ecdsa_sign_recoverable(
		secp256k1_context_sign, &sig, hash.begin(), begin(), secp256k1_nonce_function_rfc6979, nullptr
	);
	assert(ret);
	secp256k1_ecdsa_recoverable_signature_serialize_compact(secp256k1_context_sign, &vchSig[1], &rec, &sig);
	assert(ret);
	assert(rec != -1);
	vchSig[0] = 27 + rec + (fCompressed ? 4 : 0);
	return true;
}

bool CKey::Load(const CPrivKey& privkey, const CPubKey& vchPubKey, bool fSkipCheck = false)
{
	auto secp256k1_context_sign_sp = ECCVerifyHandle::secp256k1_context_sign();
	auto secp256k1_context_sign = secp256k1_context_sign_sp.get();

	if (!ec_privkey_import_der(secp256k1_context_sign, (unsigned char*) begin(), privkey.data(), privkey.size()))
	{
		return false;
	}
	fCompressed = vchPubKey.IsCompressed();
	fValid = true;

	if (fSkipCheck)
	{
		return true;
	}

	return VerifyPubKey(vchPubKey);
}

bool CKey::Derive(CKey& keyChild, ChainCode& ccChild, unsigned int nChild, const ChainCode& cc) const
{
	assert(IsValid());
	assert(IsCompressed());
	std::vector<unsigned char, secure_allocator<unsigned char>> vout(64);
	if ((nChild >> 31) == 0)
	{
		CPubKey pubkey = GetPubKey();
		assert(pubkey.size() == CPubKey::COMPRESSED_PUBLIC_KEY_SIZE);
		BIP32Hash(cc, nChild, *pubkey.begin(), pubkey.begin() + 1, vout.data());
	}
	else
	{
		assert(size() == 32);
		BIP32Hash(cc, nChild, 0, begin(), vout.data());
	}
	memcpy(ccChild.begin(), vout.data() + 32, 32);
	memcpy((unsigned char*) keyChild.begin(), begin(), 32);

	auto secp256k1_context_sign_sp = ECCVerifyHandle::secp256k1_context_sign();
	auto secp256k1_context_sign = secp256k1_context_sign_sp.get();

	bool ret = secp256k1_ec_privkey_tweak_add(secp256k1_context_sign, (unsigned char*) keyChild.begin(), vout.data());
	keyChild.fCompressed = true;
	keyChild.fValid = ret;
	return ret;
}
