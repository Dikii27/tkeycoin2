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


// Bip32Hash.hpp

#pragma once


//class Bip32Hash final
//{
//public:
//	Bip32Hash() = delete; // Default-constructor
//	Bip32Hash(Bip32Hash&&) noexcept = delete; // Move-constructor
//	Bip32Hash(const Bip32Hash&) = delete; // Copy-constructor
//	virtual ~Bip32Hash() override = default; // Destructor
//	Bip32Hash& operator=(Bip32Hash&&) noexcept = delete; // Move-assignment
//	Bip32Hash& operator=(Bip32Hash const&) = delete; // Copy-assignment
//
//	Bip32Hash()
//	{}
//};
//

#include <crypto/keys/common.hpp>

void BIP32Hash(
	const ChainCode &chainCode,
	unsigned int nChild,
	unsigned char header,
	const unsigned char data[32],
	unsigned char output[64]
);
