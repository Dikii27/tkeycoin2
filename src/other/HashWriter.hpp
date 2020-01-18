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


// HashWriter.hpp

#pragma once

//
//class HashWriter final
//{
//public:
//	HashWriter() = delete; // Default-constructor
//	HashWriter(HashWriter&&) noexcept = delete; // Move-constructor
//	HashWriter(const HashWriter&) = delete; // Copy-constructor
//	virtual ~HashWriter() override = default; // Destructor
//	HashWriter& operator=(HashWriter&&) noexcept = delete; // Move-assignment
//	HashWriter& operator=(HashWriter const&) = delete; // Copy-assignment
//
//	HashWriter()
//	{}
//};


#include <types/Blobs.hpp>
#include <serialization/SerializationActionType.hpp>
#include "Hash256.hpp"

/** A writer stream (for serialization) that computes a 256-bit hash. */
class CHashWriter : public std::ostream
{
private:
	CHash256 ctx;

	const SerializationActionType nType;
	const int nVersion;
public:

	CHashWriter(SerializationActionType nTypeIn, int nVersionIn)
	: nType(nTypeIn)
	, nVersion(nVersionIn)
	{
	}

	auto GetType() const
	{
		return nType;
	}
	int GetVersion() const
	{
		return nVersion;
	}

	void write(const char *pch, size_t size)
	{
		ctx.Write((const unsigned char*)pch, size);
	}

	// invalidates the object
	uint256 GetHash()
	{
		uint256 result;
		ctx.Finalize((unsigned char*)result.data());
		return result;
	}

	template<typename T>
	CHashWriter& operator<<(const T& obj)
	{
		// Serialize to this stream
		::Serialize(*this, obj);
		return (*this);
	}
};

