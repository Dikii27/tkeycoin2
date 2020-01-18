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


// HashVerifier.hpp

#pragma once


//class HashVerifier final
//{
//public:
//	HashVerifier() = delete; // Default-constructor
//	HashVerifier(HashVerifier&&) noexcept = delete; // Move-constructor
//	HashVerifier(const HashVerifier&) = delete; // Copy-constructor
//	virtual ~HashVerifier() override = default; // Destructor
//	HashVerifier& operator=(HashVerifier&&) noexcept = delete; // Move-assignment
//	HashVerifier& operator=(HashVerifier const&) = delete; // Copy-assignment
//
//	HashVerifier()
//	{}
//};


#include <cstddef>
#include <algorithm>
#include <serialization/Serialization.hpp>
#include "HashWriter.hpp"

/** Reads data from an underlying stream, while hashing the read data. */
template<typename Source>
class CHashVerifier : public CHashWriter
{
private:
	Source* source;

public:
	explicit CHashVerifier(Source* source_)
	: CHashWriter(
		source_->GetType(),
		source_->GetVersion()
	)
	, source(source_)
	{}

	void read(char* pch, size_t nSize)
	{
		source->read(pch, nSize);
		this->write(pch, nSize);
	}

	void ignore(size_t nSize)
	{
		char data[1024];
		while (nSize > 0) {
			size_t now = std::min<size_t>(nSize, 1024);
			read(data, now);
			nSize -= now;
		}
	}

	template<typename T>
	CHashVerifier<Source>& operator>>(T&& obj)
	{
		// Unserialize from this stream
		::Unserialize(*this, obj);
		return (*this);
	}
};
