//  Copyright (c) 2017-2019 Tkeycoin Dao. All rights reserved.
//  Copyright (c) 2019-2020 TKEY DMCC LLC & Tkeycoin Dao. All rights reserved.
//  Website: www.tkeycoin.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//


// TlvSerializer.hpp


#pragma once

#include "Serializer.hpp"

class TlvSerializer final: public Serializer
{
private:
	SVal decodeNull(std::istream& is);
	SVal decodeBool(std::istream& is);

	void putUtf8Symbol(SStr &str, uint32_t symbol);
	SVal decodeString(std::istream& is);
	SVal decodeBinary(std::istream& is);

	SVal decodeInteger(std::istream& is);
	SVal decodeFloat(std::istream& is);

	SVal decodeArray(std::istream& is);
	SVal decodeObject(std::istream& is);

	SVal decodeValue(std::istream& is);

	void encodeNull(std::ostream& os, const SVal& value);
	void encodeBool(std::ostream& os, const SVal& value);

	void encodeString(std::ostream& os, const SVal& value);
	void encodeBinary(std::ostream& os, const SVal& value);

	void encodeNumber(std::ostream& os, const SVal& value);

	void encodeArray(std::ostream& os, const SVal& value);
	void encodeKey(std::ostream& os, const std::string& key);
	void encodeObject(std::ostream& os, const SVal& value);

	void encodeValue(std::ostream& os, const SVal& value);

DECLARE_SERIALIZER(TlvSerializer);
};
