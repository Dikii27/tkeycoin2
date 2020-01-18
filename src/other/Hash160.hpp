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


// Hash160.hpp

#pragma once


//class Hash160 final
//{
//public:
//	Hash160() = delete; // Default-constructor
//	Hash160(Hash160&&) noexcept = delete; // Move-constructor
//	Hash160(const Hash160&) = delete; // Copy-constructor
//	virtual ~Hash160() override = default; // Destructor
//	Hash160& operator=(Hash160&&) noexcept = delete; // Move-assignment
//	Hash160& operator=(Hash160 const&) = delete; // Copy-assignment
//
//	Hash160()
//	{}
//};


#include <crypto/sha256.h>
#include <crypto/ripemd160.h>

/** A hasher class for Bitcoin's 160-bit hash (SHA-256 + RIPEMD-160). */
class CHash160
{
private:
	CSHA256 sha;

public:
	static const size_t OUTPUT_SIZE = CRIPEMD160::OUTPUT_SIZE;

	void Finalize(unsigned char hash[OUTPUT_SIZE])
	{
		unsigned char buf[CSHA256::OUTPUT_SIZE];
		sha.Finalize(buf);
		CRIPEMD160().Write(buf, CSHA256::OUTPUT_SIZE).Finalize(hash);
	}

	CHash160& Write(const unsigned char* data, size_t len)
	{
		sha.Write(data, len);
		return *this;
	}

	CHash160& Reset()
	{
		sha.Reset();
		return *this;
	}
};
