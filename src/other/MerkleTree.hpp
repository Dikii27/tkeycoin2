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


// MerkleTree.hpp

#pragma once


#include <crypto/sha256.h>
#include <types/Blobs.hpp>
#include <other/HashStream.hpp>
#include <cassert>

template<class Iterator, class Hasher = CSHA256>
std::vector<uint8_t> MerkleLeaves(Iterator begin, Iterator end)
{
	std::vector<uint8_t> hashes;
	hashes.reserve(end - begin);

	for (auto i = begin; i != end; i++)
	{
		HashStream<Hasher> hs{};
		::Serialize(hs, *i);

		auto& data = hs.digest();
		hashes.insert(hashes.end(), data.begin(), data.end());
	}

	return hashes;
}

template<class Hasher = CSHA256>
uint256 MerkleRoot(std::vector<uint8_t> hashes)
{
	if ((hashes.size() > Hasher::OUTPUT_SIZE) && (hashes.size() / Hasher::OUTPUT_SIZE) & 1u)
	{
		hashes.reserve(hashes.size() + Hasher::OUTPUT_SIZE);
	}

	Hasher hasher;
	while (hashes.size() > Hasher::OUTPUT_SIZE)
	{
		if ((hashes.size() / Hasher::OUTPUT_SIZE) & 1u)
		{
			std::copy(hashes.end() - Hasher::OUTPUT_SIZE, hashes.end(), std::back_inserter(hashes));
		}

		auto dst = hashes.data();
		auto src = hashes.data();

		auto end = hashes.data() + hashes.size();

		for (;;)
		{
			hasher
				.Reset()
				.Write(src, Hasher::OUTPUT_SIZE * 2)
				.Finalize(dst);
			hasher
				.Reset()
				.Write(dst, Hasher::OUTPUT_SIZE)
				.Finalize(dst);
			src += Hasher::OUTPUT_SIZE * 2;
			dst += Hasher::OUTPUT_SIZE;
			if (src >= end) break;
		}
		hashes.resize(hashes.size() / 2);
	}

	uint256 hash;
	std::copy(hashes.begin(), hashes.end(), hash.begin());

	return hash;
}

template<class Iterator, class Hasher = CSHA256>
uint256 MerkleRoot(Iterator dataBegin, Iterator dataEnd)
{
	return MerkleRoot(MerkleLeaves(dataBegin, dataEnd));
}
