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


// Serialization_test.cpp

#include "Serialization.hpp"
#include "../types/VariableLengthInteger.hpp"
#include "../types/Blob.hpp"
#include "SerializationWrapper.hpp"

#include <gtest/gtest.h>


TEST(Serialization, Substitutions)
{
//	EXPECT_TRUE(is_serializable<int>);
//	EXPECT_TRUE(is_serializable<std::vector<int>>);
//	EXPECT_TRUE(is_serializable<std::shared_ptr<int>>);
//	EXPECT_TRUE(is_serializable<uintV>);
//	EXPECT_TRUE(is_serializable<std::vector<uintV>>);
//	EXPECT_TRUE(is_serializable<std::shared_ptr<uintV>>);

	EXPECT_FALSE(is_serializable_by_method<int>::value);
	EXPECT_FALSE(is_serializable_by_method<std::vector<int>>::value);
	EXPECT_FALSE(is_serializable_by_method<std::shared_ptr<int>>::value);
	EXPECT_TRUE(is_serializable_by_method<uintV>::value);
	EXPECT_FALSE(is_serializable_by_method<std::vector<uintV>>::value);
	EXPECT_FALSE(is_serializable_by_method<std::shared_ptr<uintV>>::value);

	std::stringstream ss;

	std::shared_ptr<int>::element_type a;

	// integral
	uint8_t uint8 = 0x12;
	uint16_t uint16 = 0x1234;
	uint32_t uint32 = 0x12345678;
	uint64_t uint64 = 0x123456789abcdef;

	::Serialize(ss, uint8);
	::Serialize(ss, uint16);
	::Serialize(ss, uint32);
	::Serialize(ss, uint64);

	::Unserialize(ss, uint8);
	::Unserialize(ss, uint16);
	::Unserialize(ss, uint32);
	::Unserialize(ss, uint64);

	// Serializable
	uintV uintN(0x12345);
	Blob<24> blob("0x123456");

	::Serialize(ss, uintN);
	::Serialize(ss, blob);

	::Unserialize(ss, uintN);
	::Unserialize(ss, blob);

	// Smart pointer
	auto uint64_sp = std::make_shared<uint64_t>(0x123);
	auto uintN_sp = std::make_shared<uintV>(0x123);
	auto blob_sp = std::make_shared<Blob<24>>("0x123");

	::Serialize(ss, uint64_sp);
	::Serialize(ss, uintN_sp);
	::Serialize(ss, blob_sp);

	::Unserialize(ss, uint64_sp);
	::Unserialize(ss, uintN_sp);
	::Unserialize(ss, blob_sp);

	// Vector
	std::vector<uint64_t> vec_uint64{{0x1, 0x23, 0x456}};
	std::vector<uintV> vec_uintV{{0x1, 0x23, 0x456}};
	std::vector<Blob<24>> vec_blob{{"0x1","0x23","0x456"}};

	std::vector<std::shared_ptr<uint64_t>> vec_uint64_sp = {{std::make_shared<uint64_t>(0xde), std::make_shared<uint64_t>(0xad)}};
	std::vector<std::shared_ptr<uintV>> vec_uintV_sp{{std::make_shared<uintV>(0xde), std::make_shared<uintV>(0xad)}};
	std::vector<std::shared_ptr<Blob<24>>> vec_blob_sp{{std::make_shared<Blob<24>>("0xde"), std::make_shared<Blob<24>>("0xad")}};

	::Serialize(ss, vec_uint64);
	::Serialize(ss, vec_uintV);
	::Serialize(ss, vec_blob);
	::Serialize(ss, vec_uint64_sp);
	::Serialize(ss, vec_uintV_sp);
	::Serialize(ss, vec_blob_sp);

	::Unserialize(ss, vec_uint64);
	::Unserialize(ss, vec_uintV);
	::Unserialize(ss, vec_blob);
	::Unserialize(ss, vec_uint64_sp);
	::Unserialize(ss, vec_uintV_sp);
	::Unserialize(ss, vec_blob_sp);


	::Serialize(ss, size_and_(vec_uint64));
	::Serialize(ss, size_and_(vec_uintV));
	::Serialize(ss, size_and_(vec_blob));
	::Serialize(ss, size_and_(vec_uint64_sp));
	::Serialize(ss, size_and_(vec_uintV_sp));
	::Serialize(ss, size_and_(vec_blob_sp));

	::Unserialize(ss, size_and_(vec_uint64));
	::Unserialize(ss, size_and_(vec_uintV));
	::Unserialize(ss, size_and_(vec_blob));
	::Unserialize(ss, size_and_(vec_uint64_sp));
	::Unserialize(ss, size_and_(vec_uintV_sp));
	::Unserialize(ss, size_and_(vec_blob_sp));

	::SerializeList(ss,
		uint8,
		uint16,
		uint32,
		uint64,
		uintN,
		blob,
		uint64_sp,
		uintN_sp,
		blob_sp
	);

	::SerializeList(ss,
		vec_uint64,
		vec_uintV,
		vec_blob,
		vec_uint64_sp,
		vec_uintV_sp,
		vec_blob_sp
	);

	::SerializeList(ss,
	                size_and_(vec_uint64),
	                size_and_(vec_uintV),
	                size_and_(vec_blob),
	                size_and_(vec_uint64_sp),
	                size_and_(vec_uintV_sp),
	                size_and_(vec_blob_sp)
	);



	::UnserializeList(ss,
		uint8,
		uint16,
		uint32,
		uint64,
		uintN,
		blob,
		uint64_sp,
		uintN_sp,
		blob_sp
	);
	::UnserializeList(ss,
		vec_uint64,
		vec_uintV,
		vec_blob,
		vec_uint64_sp,
		vec_uintV_sp,
		vec_blob_sp
	);
	::UnserializeList(ss,
	                  size_and_(vec_uint64),
	                  size_and_(vec_uintV),
	                  size_and_(vec_blob),
	                  size_and_(vec_uint64_sp),
	                  size_and_(vec_uintV_sp),
	                  size_and_(vec_blob_sp)
	);

}
