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


// SerializationWrapper_test.cpp

#include "SerializationWrapper.hpp"

#include <gtest/gtest.h>

TEST(SerializationWrapper, Substitutions)
{
	{
		std::vector<char> src{'1','2','3','4','5','6','7'};
		std::vector<char> dst;

		std::stringstream ss;

		::Serialize(ss, size_and_(src));

		EXPECT_EQ(ss.str(), std::string() + (char)(src.size()) + "1234567");

		::Unserialize(ss, size_and_(dst));

		EXPECT_EQ(src, dst);
	}

	{
		std::vector<char> src{'1','2','3','4','5','6','7'};
		std::vector<char> dst{'a','b','c','d'};

		std::stringstream ss;

		::Serialize(ss, size_and_(src));

		EXPECT_EQ(ss.str(), std::string() + (char)(src.size()) + "1234567");

		::Unserialize(ss, size_and_(dst));

		EXPECT_EQ(src, dst);
	}

	{
		std::vector<char> src{'1','2','3','4'};
		std::vector<char> dst{'a','b','c','d','e','f','g'};

		std::stringstream ss;

		::Serialize(ss, size_and_(src));

		EXPECT_EQ(ss.str(), std::string() + (char)(src.size()) + "1234");

		::Unserialize(ss, size_and_(dst));

		EXPECT_EQ(src, dst);
	}

	{
		std::vector<char> src{};
		std::vector<char> dst{'a','b','c','d','e','f','g'};

		std::stringstream ss;

		::Serialize(ss, size_and_(src));

		EXPECT_EQ(ss.str(), std::string() + (char)(src.size()));

		::Unserialize(ss, size_and_(dst));

		EXPECT_EQ(src, dst);
	}

	{
		std::vector<uint8_t> src{};

		std::stringstream ss;

		::Serialize(ss, size_and_(src));
	}
}
