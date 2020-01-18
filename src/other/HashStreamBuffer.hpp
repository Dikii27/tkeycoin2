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


// HashStreamBuffer.hpp

#pragma once

#include <streambuf>
#include <array>
#include <types/Blobs.hpp>
#include <cstring>

template<class Hasher>
class HashStreamBuffer : public std::streambuf
{
private:
	std::array<char_type, 64> _bufferForWritting;
	std::array<uint8_t, Hasher::OUTPUT_SIZE> _digest;
	bool _valid;

	Hasher _hasher;
	size_t _writeCount;

public:
	HashStreamBuffer(HashStreamBuffer&&) noexcept = delete; // Move-constructor
	HashStreamBuffer(const HashStreamBuffer&) = delete; // Copy-constructor
	~HashStreamBuffer() override = default; // Destructor
	HashStreamBuffer& operator=(HashStreamBuffer&&) noexcept = delete; // Move-assignment
	HashStreamBuffer& operator=(HashStreamBuffer const&) = delete; // Copy-assignment

	HashStreamBuffer() // Default-constructor
	: _writeCount(0)
	, _valid(false)
	{
		setp(_bufferForWritting.begin(), _bufferForWritting.end());
		_digest.fill(0);
	}

	size_t size()
	{
		return _writeCount;
	}

	const std::array<uint8_t, Hasher::OUTPUT_SIZE>& digest()
	{
		if (!_valid)
		{
			sync();
			_hasher.Finalize(_digest.data());
			_hasher
				.Reset()
				.Write(_digest.data(), _digest.size())
				.Finalize(_digest.data());
			_valid = true;
		}
		return _digest;
	}

	uint256 hash()
	{
		const auto& data = digest();
		uint256 hash;
		std::copy(data.begin(), data.end(), hash.begin());
		return hash;
	}

protected:
	int overflow(int c) override
	{
		char_type c0 = c;
		if (c != traits_type::eof())
		{
			*pptr() = c; //тут нам пригодился 1 "лишний" символ, убранный в конструкторе
			pbump(1); //смещаем указатель позиции буфера на реальный конец буфера
			_writeCount++;
			return sync() == 0 ? c : traits_type::eof();
		}

		return traits_type::eof();
	}

	int sync() override
	{
		if (pptr() == pbase())
		{
			return 0;
		}

		_hasher.Write(reinterpret_cast<uint8_t*>(pbase()), pptr() - pbase());

		setp(_bufferForWritting.begin(), _bufferForWritting.end());
		return 0;
	}

	std::streamsize xsputn(const char_type* data, std::streamsize size) override
	{
		if (_valid)
		{
			_hasher.Reset();
			_writeCount = 0;
			_valid = false;
		}

		auto space = epptr() - pptr();

		if (size < space)
		{
			memcpy(pptr(), data, size);
			pbump(size);
			_writeCount += size;
			return size;
		}

		auto blockSize = epptr() - pptr();
		memcpy(pptr(), data, blockSize);

		_hasher.Write(reinterpret_cast<const uint8_t*>(_bufferForWritting.data()), _bufferForWritting.size());

		setp(_bufferForWritting.begin(), _bufferForWritting.end());

		data += blockSize;
		auto remain = size - blockSize;

		if (remain < _bufferForWritting.size())
		{
			memcpy(pptr(), data, remain);
			pbump(remain);
			_writeCount += size;
			return size;
		}

		_hasher.Write(reinterpret_cast<const uint8_t*>(data), remain);

		_writeCount += size;
		return size;
	}
};
