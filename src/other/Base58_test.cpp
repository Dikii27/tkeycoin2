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


// Base58_test.cpp

#include "Base58.hpp"

#include <gtest/gtest.h>

TEST(Base58, Encode)
{
	{
		std::vector<char> original{};

		auto encoded = Base58::EncodeBase58(original);

		EXPECT_EQ(encoded, std::string());
	}
	{
		std::vector<char> original{0,0,0,0,0};

		auto encoded = Base58::EncodeBase58(original);

		EXPECT_EQ(encoded, std::string("11111"));
	}
	{
		std::vector<char> original{'1', '2', '3'};

		auto encoded = Base58::EncodeBase58(original);

		EXPECT_EQ(encoded, std::string("HXRC"));
	}
	{
		std::vector<char> original{0, '1', '2', '3'};

		auto encoded = Base58::EncodeBase58(original);

		EXPECT_EQ(encoded, std::string("1HXRC"));
	}
	{
		auto original = "The quick brown fox jumps over the lazy dog";

		auto encoded = Base58::EncodeBase58(original);

		EXPECT_EQ(encoded, std::string("7DdiPPYtxLjCD3wA1po2rvZHTDYjkZYiEtazrfiwJcwnKCizhGFhBGHeRdx"));
	}
}

TEST(Base58, Decode)
{
	{
		std::string original;
		std::vector<uint8_t> decoded;

		Base58::DecodeBase58(original, decoded);

		EXPECT_EQ(decoded, std::vector<uint8_t>{});
	}
	{
		std::string original("11111");
		std::vector<uint8_t> expect({0,0,0,0,0});

		std::vector<uint8_t> decoded;
		Base58::DecodeBase58(original, decoded);

		EXPECT_EQ(decoded, expect);
	}
	{
		std::string original("HXRC");
		std::vector<uint8_t> expect({'1', '2', '3'});

		std::vector<uint8_t> decoded;
		Base58::DecodeBase58(original, decoded);

		EXPECT_EQ(decoded, expect);
	}
	{
		std::string original("1HXRC");
		std::vector<uint8_t> expect({0, '1', '2', '3'});

		std::vector<uint8_t> decoded;
		Base58::DecodeBase58(original, decoded);

		EXPECT_EQ(decoded, expect);
	}
	{
		std::string original("7DdiPPYtxLjCD3wA1po2rvZHTDYjkZYiEtazrfiwJcwnKCizhGFhBGHeRdx");
		std::string expect_s = "The quick brown fox jumps over the lazy dog";

		std::vector<uint8_t> decoded;
		Base58::DecodeBase58(original, decoded);

		std::string decoded_s(decoded.begin(), decoded.end());

		EXPECT_EQ(decoded_s, expect_s);
	}
}
