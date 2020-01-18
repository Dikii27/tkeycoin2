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


// ScriptData.hpp

#pragma once


#include <vector>
#include <cstdint>
#include <array>

class ScriptData : public std::vector<uint8_t>
{
public:
	ScriptData() = default; // Default-constructor
	ScriptData(ScriptData&&) noexcept = default; // Move-constructor
	ScriptData(const ScriptData&) = default; // Copy-constructor
	~ScriptData() = default; // Destructor
	ScriptData& operator=(ScriptData&&) noexcept = default; // Move-assignment
	ScriptData& operator=(ScriptData const&) = default; // Copy-assignment

	ScriptData(std::vector<uint8_t>&& vector)
	: std::vector<uint8_t>(std::forward<std::vector<uint8_t>>(vector))
	{
	}

	ScriptData(const std::vector<uint8_t>& vector)
	: std::vector<uint8_t>(vector)
	{
	}

	ScriptData(const uint8_t* data, size_t size)
	: std::vector<uint8_t>(data, data + size)
	{
	}

	ScriptData(size_t size)
	: std::vector<uint8_t>(size)
	{
	}
};


