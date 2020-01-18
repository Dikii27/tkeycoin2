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


// OutputConnectionStreamBuffer.hpp

#pragma once


#include <streambuf>
#include <utils/Writer.hpp>

class OutputConnectionStreamBuffer : public std::streambuf
{
private:
	Writer& _writer;

public:
	OutputConnectionStreamBuffer(OutputConnectionStreamBuffer&&) noexcept = delete; // Move-constructor
	OutputConnectionStreamBuffer(const OutputConnectionStreamBuffer&) = delete; // Copy-constructor
	~OutputConnectionStreamBuffer() override = default; // Destructor
	OutputConnectionStreamBuffer& operator=(OutputConnectionStreamBuffer&&) noexcept = delete; // Move-assignment
	OutputConnectionStreamBuffer& operator=(OutputConnectionStreamBuffer const&) = delete; // Copy-assignment

	OutputConnectionStreamBuffer(Writer& writer) // Default-constructor
	: _writer(writer)
	{
	}

protected:
	int overflow(int c) override
	{
		return c;
	}

	int sync() override
	{
		if (pptr() == pbase())
		{
			return 0;
		}

		_writer.write(pbase(), pptr() - pbase());
		return 0;
	}

	std::streamsize xsputn(const char_type* data, std::streamsize size) override
	{
		_writer.write(data, size);
		return size;
	}
};

