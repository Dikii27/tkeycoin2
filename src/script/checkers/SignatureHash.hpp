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


// SignatureHash.hpp

#pragma once

//
//class SignatureHash final
//{
//public:
//	SignatureHash() = delete; // Default-constructor
//	SignatureHash(SignatureHash&&) noexcept = delete; // Move-constructor
//	SignatureHash(const SignatureHash&) = delete; // Copy-constructor
//	virtual ~SignatureHash() override = default; // Destructor
//	SignatureHash& operator=(SignatureHash&&) noexcept = delete; // Move-assignment
//	SignatureHash& operator=(SignatureHash const&) = delete; // Copy-assignment
//
//	SignatureHash()
//	{}
//};

#include <types/Flags.hpp>
#include "SignatureHashType.hpp"
#include "SignatureChecker.hpp"
#include "PrecomputedTransactionData.hpp"

template<class T>
uint256 SignatureHash(
	const Script& scriptCode,
	const T& tx,
	unsigned int nIn,
	Flags<SignatureHashType> nHashType,
	const Amount& amount,
	SigVersion sigversion,
	const PrecomputedTransactionData* cache = nullptr
);
