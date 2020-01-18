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


// BlobTest.cpp

#include "Blob.hpp"

#include <gtest/gtest.h>

TEST(Blob, Substitutions)
{
	EXPECT_EQ(Blob<1>::bits, 1);
	EXPECT_EQ(Blob<1>::bytes, 1);

	EXPECT_EQ(Blob<7>::bits, 7);
	EXPECT_EQ(Blob<7>::bytes, 1);

	EXPECT_EQ(Blob<8>::bits, 8);
	EXPECT_EQ(Blob<8>::bytes, 1);

	EXPECT_EQ(Blob<9>::bits, 9);
	EXPECT_EQ(Blob<9>::bytes, 2);

	EXPECT_EQ(Blob<31>::bits, 31);
	EXPECT_EQ(Blob<31>::bytes, 4);

	EXPECT_EQ(Blob<32>::bits, 32);
	EXPECT_EQ(Blob<32>::bytes, 4);

	EXPECT_EQ(Blob<33>::bits, 33);
	EXPECT_EQ(Blob<33>::bytes, 5);
}

using blob24 = Blob<24>;

TEST(Blob, Construction)
{
	{
		blob24 blob;
		EXPECT_EQ(blob[0], 0);
		EXPECT_EQ(blob[1], 0);
		EXPECT_EQ(blob[2], 0);
	}

	{
		blob24 blob(std::vector<uint8_t>{0xde, 0xad, 0x00});
		EXPECT_FALSE(blob.isNull());
		EXPECT_EQ(blob[0], 0x00);
		EXPECT_EQ(blob[1], 0xad);
		EXPECT_EQ(blob[2], 0xde);
	}

	{
		blob24 blob(std::vector<uint8_t>{0x00, 0xde, 0xad});
		EXPECT_FALSE(blob.isNull());
		EXPECT_EQ(blob[0], 0xad);
		EXPECT_EQ(blob[1], 0xde);
		EXPECT_EQ(blob[2], 0x00);
	}

	{
		blob24 blob(std::vector<uint8_t>{0xab, 0x00, 0xcd});
		EXPECT_FALSE(blob.isNull());
		EXPECT_EQ(blob[0], 0xcd);
		EXPECT_EQ(blob[1], 0x00);
		EXPECT_EQ(blob[2], 0xab);
	}

	{
		blob24 blob(std::vector<uint8_t>{0xab, 0xcd, 0xef});
		EXPECT_FALSE(blob.isNull());
		EXPECT_EQ(blob[0], 0xef);
		EXPECT_EQ(blob[1], 0xcd);
		EXPECT_EQ(blob[2], 0xab);
	}

	{
		blob24 blob("123");
		EXPECT_FALSE(blob.isNull());
		EXPECT_EQ(blob[0], 0x23);
		EXPECT_EQ(blob[1], 0x01);
		EXPECT_EQ(blob[2], 0x00);
	}

	{
		blob24 blob("0x1234");
		EXPECT_FALSE(blob.isNull());
		EXPECT_EQ(blob[0], 0x34);
		EXPECT_EQ(blob[1], 0x12);
		EXPECT_EQ(blob[2], 0x00);
	}

	{
		blob24 blob("0x12345");
		EXPECT_FALSE(blob.isNull());
		EXPECT_EQ(blob[0], 0x45);
		EXPECT_EQ(blob[1], 0x23);
		EXPECT_EQ(blob[2], 0x01);
	}

	{
		blob24 blob("0x123456");
		EXPECT_FALSE(blob.isNull());
		EXPECT_EQ(blob[0], 0x56);
		EXPECT_EQ(blob[1], 0x34);
		EXPECT_EQ(blob[2], 0x12);
	}

	{
		blob24 blob("0x1234567");
		EXPECT_FALSE(blob.isNull());
		EXPECT_EQ(blob[0], 0x67);
		EXPECT_EQ(blob[1], 0x45);
		EXPECT_EQ(blob[2], 0x23);
	}
}

TEST(Blob, StringRepresentation)
{
	{
		blob24 blob;
		EXPECT_EQ(blob.str(), "000000");
	}

	{
		blob24 blob("0x1");
		EXPECT_EQ(blob.str(), "000001");
	}

	{
		blob24 blob("0x00000000dEAd");
		EXPECT_EQ(blob.str(), "00dead");
	}

	{
		blob24 blob("0xDeaD00");
		EXPECT_EQ(blob.str(), "dead00");
	}

	{
		blob24 blob("0xDEAD0");
		EXPECT_EQ(blob.str(), "0dead0");
	}
}

TEST(Blob, Compare)
{
	{
		blob24 blob1("0x000001");
		blob24 blob2("0x000002");
		EXPECT_LT(blob1, blob2);
	}
	{
		blob24 blob1("0x000102");
		blob24 blob2("0x000201");
		EXPECT_LT(blob1, blob2);
	}
	{
		blob24 blob1("0x010002");
		blob24 blob2("0x020001");
		EXPECT_LT(blob1, blob2);
	}

	{
		blob24 blob1("0x000001");
		blob24 blob2("0x000002");
		EXPECT_GT(blob2, blob1);
	}
	{
		blob24 blob1("0x000102");
		blob24 blob2("0x000201");
		EXPECT_GT(blob2, blob1);
	}
	{
		blob24 blob1("0x010002");
		blob24 blob2("0x020001");
		EXPECT_GT(blob2, blob1);
	}
}
