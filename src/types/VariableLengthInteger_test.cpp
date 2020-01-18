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


// VariableLengthInteger_test.cpp

#include "VariableLengthInteger.hpp"

#include <gtest/gtest.h>

TEST(VariableLengthInteger, Constructor)
{
	// default
	{ uintV n;              EXPECT_EQ((int)n, 0); }

	// ctor
	{ uintV n(0);       	EXPECT_EQ((int)n, 0); }
	{ uintV n(0xf);         EXPECT_EQ((int)n, 0xf); }
	{ uintV n(0xff);        EXPECT_EQ((int)n, 0xff); }
	{ uintV n(0xfff);       EXPECT_EQ((int)n, 0xfff); }
	{ uintV n(0xffff);      EXPECT_EQ((int)n, 0xffff); }
	{ uintV n(0xfffffff);   EXPECT_EQ((int)n, 0xfffffff); }
	{ uintV n(0xffffffff);  EXPECT_EQ((int)n, 0xffffffff); }
	{
		uintV n = 0xdead;
		EXPECT_EQ((int)n, 0xdead);

		n = 0xbeef;
		EXPECT_EQ((int)n, 0xbeef);

		uintV cc1(n); // copy-ctor
		EXPECT_EQ((int) cc1, 0xbeef);

		uintV cc2 = n; // copy-ctor
		EXPECT_EQ((int) cc2, 0xbeef);

		n = 0xbeef;
		uintV mc1 = std::move(n); // move-ctor
		EXPECT_EQ((int) mc1, 0xbeef);

		n = 0xdead;
		uintV mc2(std::move(n)); // move-ctor
		EXPECT_EQ((int)mc2, 0xdead);

		n = 0xdeadbeef;
		uintV ca;
		ca = n; // copy-assignment
		EXPECT_EQ((int) ca, 0xdeadbeef);

		n = 0xdeadbeef;
		uintV ma;
		ma = std::move(n); // move-assignment
		EXPECT_EQ((int) ma, 0xdeadbeef);
	}
}

TEST(VariableLengthInteger, Serialization)
{
	auto sizeOf = [](const uintV& n)->size_t
	{
		std::ostringstream ss;
		n.Serialize(ss);
		return ss.tellp();
	};

	// n < 253 => 1
	{ uintV n(0);       	        EXPECT_EQ(sizeOf(n), 1); }
	{ uintV n(1);                   EXPECT_EQ(sizeOf(n), 1); }
	{ uintV n(252);                 EXPECT_EQ(sizeOf(n), 1); }
	// n = 253..0xffff => 3 (1+2)
	{ uintV n(253);                 EXPECT_EQ(sizeOf(n), 3); }
	{ uintV n(0xffff);              EXPECT_EQ(sizeOf(n), 3); }
	// n = 0x10000..0xffffffff => 5 (1+4)
	{ uintV n(0x10000);             EXPECT_EQ(sizeOf(n), 5); }
	{ uintV n(0xffffffff);          EXPECT_EQ(sizeOf(n), 5); }
	// n = 0x100000000..0xffffffffffffffff => 9 (1+8)
	{ uintV n(0x100000000);         EXPECT_EQ(sizeOf(n), 9); }
	{ uintV n(0xffffffffffffffff);  EXPECT_EQ(sizeOf(n), 9); }
}
