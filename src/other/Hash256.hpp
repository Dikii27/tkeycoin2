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


// Hash256.hpp

#pragma once




#include <crypto/sha256.h>

/** A hasher class for Bitcoin's 256-bit hash (double SHA-256). */
class CHash256
{
private:
	CSHA256 sha;

public:
	static const size_t OUTPUT_SIZE = CSHA256::OUTPUT_SIZE;

	void Finalize(unsigned char hash[OUTPUT_SIZE])
	{
		unsigned char buf[OUTPUT_SIZE];
		sha.Finalize(buf);
		sha.Reset();
		sha.Write(buf, OUTPUT_SIZE);
		sha.Finalize(hash);
	}

	CHash256& Write(const unsigned char* data, size_t len)
	{
		sha.Write(data, len);
		return *this;
	}

	CHash256& Reset()
	{
		sha.Reset();
		return *this;
	}
};
