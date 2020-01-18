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


// EccVerifyHandle.cpp

#include <secp256k1.h>
#include <cassert>
#include <support/Random.hpp>
#include "EclipticCurveContext.hpp"
#include "CKey.hpp"
#include "CPubKey.hpp"

ECCVerifyHandle::ECCVerifyHandle()
{
	_secp256k1_context_verify.reset(secp256k1_context_create(SECP256K1_CONTEXT_VERIFY), secp256k1_context_destroy);

	_secp256k1_context_sign.reset(secp256k1_context_create(SECP256K1_CONTEXT_SIGN), secp256k1_context_destroy);

	// Pass in a random blinding seed to the secp256k1 context.
	std::vector<uint8_t, secure_allocator<uint8_t>> seed(32);
	GetRandBytes(seed.data(), 32);
	bool ret = secp256k1_context_randomize(_secp256k1_context_sign.get(), seed.data());
	assert(ret);
}

bool ECCVerifyHandle::InitSanityCheck()
{
	CKey key;
	key.MakeNewKey(true);
	CPubKey pubkey = key.GetPubKey();
	return key.VerifyPubKey(pubkey);
}
