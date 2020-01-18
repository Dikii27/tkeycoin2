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


// Hex_test.cpp

#include "Hex.hpp"

#include <gtest/gtest.h>

TEST(Hex, HexDetect)
{
	auto msg1 = "This is correct hex-string";
	EXPECT_TRUE(Hex::isHexString("00")) << msg1;
	EXPECT_TRUE(Hex::isHexString("01")) << msg1;
	EXPECT_TRUE(Hex::isHexString("3210")) << msg1;
	EXPECT_TRUE(Hex::isHexString("abcd")) << msg1;
	EXPECT_TRUE(Hex::isHexString("0123456789abcdefABCDEF")) << msg1;

	auto msg2 = "Empty string isn't correct hex-string";
	EXPECT_FALSE(Hex::isHexString("")) << msg2;

	auto msg3 = "String with non even lenght isn't correct hex-string";
	EXPECT_FALSE(Hex::isHexString("0")) << msg3;
	EXPECT_FALSE(Hex::isHexString("1")) << msg3;
	EXPECT_FALSE(Hex::isHexString("001")) << msg3;
	EXPECT_FALSE(Hex::isHexString("abc")) << msg3;

	auto msg4 = "String contains any non hex-digit symbol isn't correct hex-string";
	EXPECT_FALSE(Hex::isHexString("1z")) << msg4;
	EXPECT_FALSE(Hex::isHexString("q2")) << msg4;
	EXPECT_FALSE(Hex::isHexString("w34r")) << msg4;
	EXPECT_FALSE(Hex::isHexString("r567")) << msg4;
	EXPECT_FALSE(Hex::isHexString("890t")) << msg4;
	EXPECT_FALSE(Hex::isHexString("0zz0")) << msg4;
}

TEST(Hex, Parse)
{
	{
		auto actual = Hex::Parse("");
		auto expect = std::vector<uint8_t>();
		EXPECT_EQ(expect, actual);
	}
	{
		auto actual = Hex::Parse("0123456789abcdefABCDEF");
		auto expect = std::vector<uint8_t>({0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xab,0xcd,0xef});
		EXPECT_EQ(expect, actual);
	}
	{
		auto actual = Hex::Parse(" \n\t0123456789");
		auto expect = std::vector<uint8_t>({0x01,0x23,0x45,0x67,0x89});
		EXPECT_EQ(expect, actual);
	}
	{
		auto actual = Hex::Parse("01 234\t567\n89");
		auto expect = std::vector<uint8_t>({0x01,0x23,0x45,0x67,0x89});
		EXPECT_EQ(expect, actual);
	}
	{
		auto actual = Hex::Parse("z0123456789");
		auto expect = std::vector<uint8_t>();
		EXPECT_EQ(expect, actual);
	}
	{
		auto actual = Hex::Parse("012345z6789");
		auto expect = std::vector<uint8_t>({0x01, 0x23, 0x45});
		EXPECT_EQ(expect, actual);
	}
	{
		auto actual = Hex::Parse("0123456789z");
		auto expect = std::vector<uint8_t>({0x01, 0x23, 0x45, 0x67, 0x89});
		EXPECT_EQ(expect, actual);
	}
}
