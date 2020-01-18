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


// Random.hpp

#pragma once


#include <cstdint>
#include <types/Blobs.hpp>
#include <mutex>


/** Initialize the RNG. */
void RandomInit();

class Random final
{
public:
	Random(Random&&) noexcept = delete; // Move-constructor
	Random(const Random&) = delete; // Copy-constructor
	Random& operator=(Random&&) noexcept = delete; // Move-assignment
	Random& operator=(Random const&) = delete; // Copy-assignment

private:
	Random() // Default-constructor
	{
		RandomInit();
	}
	~Random() = default; // Destructor

	static Random &getInstance()
	{
		static Random instance;
		return instance;
	}

public:
//	Function to gather random data from multiple sources, failing whenever any of those sources fail to provide a result.
	static void GetStrongRandBytes(unsigned char* buf, int num);
};

/* Seed OpenSSL PRNG with additional entropy data */
void RandAddSeed();

/**
 * Functions to gather random data via the OpenSSL PRNG
 */
void GetRandBytes(unsigned char* buf, int num);

uint64_t GetRand(uint64_t nMax = std::numeric_limits<uint64_t>::max());

int GetRandInt(int nMax);

uint256 GetRandHash();

/**
 * Add a little bit of randomness to the output of GetStrongRangBytes.
 * This sleeps for a millisecond, so should only be called when there is
 * no other work to be done.
 */
void RandAddSeedSleep();


///**
// * Fast randomness source. This is seeded once with secure random data, but
// * is completely deterministic and insecure after that.
// * This class is not thread-safe.
// */
//class FastRandomContext
//{
//private:
//	bool requires_seed;
//	ChaCha20 rng;
//
//	unsigned char bytebuf[64];
//	int bytebuf_size;
//
//	uint64_t bitbuf;
//	int bitbuf_size;
//
//	void RandomSeed();
//
//	void FillByteBuffer()
//	{
//		if (requires_seed)
//		{
//			RandomSeed();
//		}
//		rng.Output(bytebuf, sizeof(bytebuf));
//		bytebuf_size = sizeof(bytebuf);
//	}
//
//	void FillBitBuffer()
//	{
//		bitbuf = rand64();
//		bitbuf_size = 64;
//	}
//
//public:
//	explicit FastRandomContext(bool fDeterministic = false);
//
//	/** Initialize with explicit seed (only for testing) */
//	explicit FastRandomContext(const uint256& seed);
//
//	/** Generate a random 64-bit integer. */
//	uint64_t rand64()
//	{
//		if (bytebuf_size < 8)
//		{ FillByteBuffer(); }
//		uint64_t ret = ReadLE64(bytebuf + 64 - bytebuf_size);
//		bytebuf_size -= 8;
//		return ret;
//	}
//
//	/** Generate a random (bits)-bit integer. */
//	uint64_t randbits(int bits)
//	{
//		if (bits == 0)
//		{
//			return 0;
//		}
//		else if (bits > 32)
//		{
//			return rand64() >> (64 - bits);
//		}
//		else
//		{
//			if (bitbuf_size < bits)
//			{ FillBitBuffer(); }
//			uint64_t ret = bitbuf & (~(uint64_t) 0 >> (64 - bits));
//			bitbuf >>= bits;
//			bitbuf_size -= bits;
//			return ret;
//		}
//	}
//
//	/** Generate a random integer in the range [0..range). */
//	uint64_t randrange(uint64_t range)
//	{
//		--range;
//		int bits = CountBits(range);
//		while (true)
//		{
//			uint64_t ret = randbits(bits);
//			if (ret <= range)
//			{ return ret; }
//		}
//	}
//
//	/** Generate random bytes. */
//	std::vector<unsigned char> randbytes(size_t len);
//
//	/** Generate a random 32-bit integer. */
//	uint32_t rand32()
//	{ return randbits(32); }
//
//	/** generate a random uint256. */
//	uint256 rand256();
//
//	/** Generate a random boolean. */
//	bool randbool()
//	{ return randbits(1); }
//
//	// Compatibility with the C++11 UniformRandomBitGenerator concept
//	typedef uint64_t result_type;
//
//	static constexpr uint64_t min()
//	{ return 0; }
//
//	static constexpr uint64_t max()
//	{ return std::numeric_limits<uint64_t>::max(); }
//
//	inline uint64_t operator()()
//	{ return rand64(); }
//};

/* Number of random bytes returned by GetOSRand.
 * When changing this constant make sure to change all call sites, and make
 * sure that the underlying OS APIs for all platforms support the number.
 * (many cap out at 256 bytes).
 */
static const int NUM_OS_RANDOM_BYTES = 32;

/** Get 32 bytes of system entropy. Do not use this in application code: use
 * GetStrongRandBytes instead.
 */
void GetOSRand(unsigned char* ent32);

/** Check that OS randomness is available and returning the requested number
 * of bytes.
 */
bool Random_SanityCheck();

