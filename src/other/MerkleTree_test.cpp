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


// MerkleTree_test.cpp

#include "MerkleTree.hpp"
#include "HashStreams.hpp"

#include <gtest/gtest.h>

class Merkle : public ::testing::Test
{
protected:
	void SetUp() override
	{
		{
			str1 = "The quick brown fox jumps over the lazy dog";
			Hash256Stream hs;
			::Serialize(hs, str1);
			data1 = hs.digest();
			hash1 = hs.hash();
		}
		{
			str2 = "Now is the time for all good men to come to the aid of the party";
			Hash256Stream hs;
			::Serialize(hs, str2);
			data2 = hs.digest();
			hash2 = hs.hash();
		}
		{
			str3 = "LITTERA SCRIPTA MANET";
			Hash256Stream hs;
			::Serialize(hs, str3);
			data3 = hs.digest();
			hash3 = hs.hash();
		}
		{
			std::vector<uint8_t> data;
			std::copy(data1.begin(), data1.end(), std::back_inserter(data));
			std::copy(data2.begin(), data2.end(), std::back_inserter(data));
			Hash256Stream hs;
			::Serialize(hs, data);
			data12 = hs.digest();
			hash12 = hs.hash();
		}
		{
			std::vector<uint8_t> data;
			std::copy(data3.begin(), data3.end(), std::back_inserter(data));
			std::copy(data3.begin(), data3.end(), std::back_inserter(data));
			Hash256Stream hs;
			::Serialize(hs, data);
			data33 = hs.digest();
			hash33 = hs.hash();
		}
		{
			std::vector<uint8_t> data;
			std::copy(data12.begin(), data12.end(), std::back_inserter(data));
			std::copy(data33.begin(), data33.end(), std::back_inserter(data));
			Hash256Stream hs;
			::Serialize(hs, data);
			hash1233 = hs.hash();
		}
	}

	std::string str1;
	std::string str2;
	std::string str3;

	std::array<uint8_t, 32> data1;
	std::array<uint8_t, 32> data2;
	std::array<uint8_t, 32> data3;
	std::array<uint8_t, 32> data12;
	std::array<uint8_t, 32> data33;

	uint256 hash1;
	uint256 hash2;
	uint256 hash3;
	uint256 hash12;
	uint256 hash33;
	uint256 hash1233;
};

TEST_F(Merkle, Leaves)
{
	{
		auto items = {str1};

		std::vector<uint8_t> expected;
		std::copy(data1.begin(), data1.end(), std::back_inserter(expected));

		auto actual = MerkleLeaves(items.begin(), items.end());

		EXPECT_EQ(expected.size(), actual.size());
		EXPECT_EQ(expected, actual);
	}
	{
		auto items = {str1, str2};

		std::vector<uint8_t> expected;
		std::copy(data1.begin(), data1.end(), std::back_inserter(expected));
		std::copy(data2.begin(), data2.end(), std::back_inserter(expected));

		auto actual = MerkleLeaves(items.begin(), items.end());

		EXPECT_EQ(expected.size(), actual.size());
		EXPECT_EQ(expected, actual);
	}
	{
		auto items = {str1, str2, str3};

		std::vector<uint8_t> expected;
		std::copy(data1.begin(), data1.end(), std::back_inserter(expected));
		std::copy(data2.begin(), data2.end(), std::back_inserter(expected));
		std::copy(data3.begin(), data3.end(), std::back_inserter(expected));

		auto actual = MerkleLeaves(items.begin(), items.end());

		EXPECT_EQ(expected.size(), actual.size());
		EXPECT_EQ(expected, actual);
	}
}

TEST_F(Merkle, Tree)
{
	{// 1
		std::vector<uint8_t> data;
		std::copy(data1.begin(), data1.end(), std::back_inserter(data));

		uint256 expected = hash1;

		uint256 actual = MerkleRoot(data);
		std::reverse(actual.begin(), actual.end());

		EXPECT_EQ(expected.str(), actual.str());
	}

	{// 1 2
		std::vector<uint8_t> data;
		std::copy(data1.begin(), data1.end(), std::back_inserter(data));
		std::copy(data2.begin(), data2.end(), std::back_inserter(data));

		// 12
		uint256 expected = hash12;

		uint256 actual = MerkleRoot(data);
		std::reverse(actual.begin(), actual.end());

		EXPECT_EQ(expected.str(), actual.str());
	}

	{// 3 3
		std::vector<uint8_t> data;
		std::copy(data3.begin(), data3.end(), std::back_inserter(data));
		std::copy(data3.begin(), data3.end(), std::back_inserter(data));

		Hash256Stream hs;
		::Serialize(hs, data);

		// 33
		uint256 expected = hash33;

		uint256 actual = MerkleRoot(data);
		std::reverse(actual.begin(), actual.end());

		EXPECT_EQ(expected.str(), actual.str());
	}

	{ // 1 2 3
		std::vector<uint8_t> data;
		std::copy(data1.begin(), data1.end(), std::back_inserter(data));
		std::copy(data2.begin(), data2.end(), std::back_inserter(data));
		std::copy(data3.begin(), data3.end(), std::back_inserter(data));

		// 1233
		uint256 expected = hash1233;

		uint256 actual = MerkleRoot(data);
		std::reverse(actual.begin(), actual.end());

		EXPECT_EQ(expected.str(), actual.str());
	}
}
