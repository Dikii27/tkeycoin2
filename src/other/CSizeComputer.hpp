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


// CSizeComputer.hpp

#pragma once


//class CSizeComputer final
//{
//public:
//	CSizeComputer() = delete; // Default-constructor
//	CSizeComputer(CSizeComputer&&) noexcept = delete; // Move-constructor
//	CSizeComputer(const CSizeComputer&) = delete; // Copy-constructor
//	virtual ~CSizeComputer() override = default; // Destructor
//	CSizeComputer& operator=(CSizeComputer&&) noexcept = delete; // Move-assignment
//	CSizeComputer& operator=(CSizeComputer const&) = delete; // Copy-assignment
//
//	CSizeComputer()
//	{}
//};


#include <cstddef>
#include <serialization/Serialization.hpp>

/* ::GetSerializeSize implementations
 *
 * Computing the serialized size of objects is done through a special stream
 * object of type CSizeComputer, which only records the number of bytes written
 * to it.
 *
 * If your Serialize or SerializationOp method has non-trivial overhead for
 * serialization, it may be worthwhile to implement a specialized version for
 * CSizeComputer, which uses the s.seek() method to record bytes that would
 * be written instead.
 */
class CSizeComputer
{
protected:
	size_t nSize;

	const int nVersion;
public:
	explicit CSizeComputer(int nVersionIn)
	: nSize(0), nVersion(nVersionIn)
	{
	}

	void write(const char* psz, size_t _nSize)
	{
		this->nSize += _nSize;
	}

	// Pretend _nSize bytes are written, without specifying them
	void seek(size_t _nSize)
	{
		this->nSize += _nSize;
	}

	template<typename T>
	CSizeComputer& operator<<(const T& obj)
	{
		::Serialize(*this, obj);
		return (*this);
	}

	size_t size() const
	{
		return nSize;
	}

	int GetVersion() const
	{
		return nVersion;
	}
};
