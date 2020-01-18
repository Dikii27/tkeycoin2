// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
//  Copyright (c) 2017-2020 TKEY DMCC LLC & Tkeycoin Dao. All rights reserved.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <crypto/ripemd160.h>
#include <crypto/sha256.h>
//#include <prevector.h>
//#include <serialize.h>
//#include <uint256.h>
//#include <version.h>

#include <vector>
#include <types/Blobs.hpp>
#include "Hash256.hpp"
#include "Hash160.hpp"

/** Compute the 256-bit hash of an object. */
template<typename T, typename std::enable_if_t<std::is_pointer_v<T>, void>* = nullptr>
inline uint256 Hash(const T begin, const T end)
{
	uint256 result;

	CHash256()
		.Write((unsigned char*)begin, (unsigned char*)end - (unsigned char*)begin)
		.Finalize((unsigned char*)result.data());

	return result;
}

template<typename T, typename std::enable_if_t<!std::is_pointer_v<T>, void>* = nullptr>
inline uint256 Hash(const T begin, const T end)
{
	uint256 result;

	CHash256()
		.Write((unsigned char*)&begin, end - begin)
		.Finalize((unsigned char*)result.data());

	return result;
}

/** Compute the 256-bit hash of the concatenation of two objects. */
template<typename T1, typename T2>
inline uint256 Hash(
	const T1 p1begin, const T1 p1end,
	const T2 p2begin, const T2 p2end
)
{
	static const unsigned char pblank[1] = {};
	uint256 result;
	CHash256()
		.Write(p1begin == p1end ? pblank : (const unsigned char*) &p1begin[0], (p1end - p1begin) * sizeof(p1begin[0]))
		.Write(p2begin == p2end ? pblank : (const unsigned char*) &p2begin[0], (p2end - p2begin) * sizeof(p2begin[0]))
		.Finalize((unsigned char*) &result);
	return result;
}

/** Compute the 160-bit hash an object. */
template<typename T1>
inline uint160 Hash160(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1] = {};
	uint160 result;
	CHash160()
		.Write(pbegin == pend ? pblank : (const unsigned char*) &pbegin[0], (pend - pbegin) * sizeof(pbegin[0]))
		.Finalize((unsigned char*) &result);
	return result;
}

/** Compute the 160-bit hash of a vector. */
inline uint160 Hash160(const std::vector<unsigned char>& vch)
{
	return Hash160(vch.begin(), vch.end());
}
//
///** Compute the 256-bit hash of an object's serialization. */
//template<typename T>
//uint256 SerializeHash(const T& obj, int nType=SER_GETHASH, int nVersion=PROTOCOL_VERSION)
//{
//    CHashWriter ss(nType, nVersion);
//    ss << obj;
//    return ss.GetHash();
//}
//
//unsigned int MurmurHash3(unsigned int nHashSeed, const std::vector<unsigned char>& vDataToHash);
//
//void BIP32Hash(
//	const ChainCode& chainCode, unsigned int nChild, unsigned char header, const unsigned char data[32], unsigned char output[64]
//);
//
///** SipHash-2-4 */
//class CSipHasher
//{
//private:
//	uint64_t v[4];
//	uint64_t tmp;
//	int count;
//
//public:
//	/** Construct a SipHash calculator initialized with 128-bit key (k0, k1) */
//	CSipHasher(uint64_t k0, uint64_t k1);
//
//	/** Hash a 64-bit integer worth of data
//	 *  It is treated as if this was the little-endian interpretation of 8 bytes.
//	 *  This function can only be used when a multiple of 8 bytes have been written so far.
//	 */
//	CSipHasher& Write(uint64_t data);
//
//	/** Hash arbitrary bytes. */
//	CSipHasher& Write(const unsigned char* data, size_t size);
//
//	/** Compute the 64-bit SipHash-2-4 of the data written so far. The object remains untouched. */
//	uint64_t Finalize() const;
//};
//
///** Optimized SipHash-2-4 implementation for uint256.
// *
// *  It is identical to:
// *    SipHasher(k0, k1)
// *      .Write(val.GetUint64(0))
// *      .Write(val.GetUint64(1))
// *      .Write(val.GetUint64(2))
// *      .Write(val.GetUint64(3))
// *      .Finalize()
// */
//uint64_t SipHashUint256(uint64_t k0, uint64_t k1, const uint256& val);
//
//uint64_t SipHashUint256Extra(uint64_t k0, uint64_t k1, const uint256& val, uint32_t extra);
