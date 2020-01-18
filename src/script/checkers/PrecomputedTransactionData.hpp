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


// PrecomputedTransactionData.hpp

#pragma once


//class PrecomputedTransactionData final
//{
//public:
//	PrecomputedTransactionData() = delete; // Default-constructor
//	PrecomputedTransactionData(PrecomputedTransactionData&&) noexcept = delete; // Move-constructor
//	PrecomputedTransactionData(const PrecomputedTransactionData&) = delete; // Copy-constructor
//	virtual ~PrecomputedTransactionData() override = default; // Destructor
//	PrecomputedTransactionData& operator=(PrecomputedTransactionData&&) noexcept = delete; // Move-assignment
//	PrecomputedTransactionData& operator=(PrecomputedTransactionData const&) = delete; // Copy-assignment
//
//	PrecomputedTransactionData()
//	{}
//};


#include <types/Blobs.hpp>

struct PrecomputedTransactionData
{
    uint256 hashPrevouts;
	uint256 hashSequence;
	uint256 hashOutputs;
    bool ready = false;

    template <class T>
    explicit PrecomputedTransactionData(const T& tx);
};

