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


// InventoryVector.cpp

#include <serialization/SObj.hpp>
#include "InventoryVector.hpp"

void protocol::InventoryVector::Serialize(std::ostream& os) const
{
	SerializeList(
		os,
		static_cast<uint32_t>(_type),   // Identifies the object type linked to this inventory
		_hash                           // Hash of the object
	);
}

void protocol::InventoryVector::Unserialize(std::istream& is)
{
	uint32_t type;
	UnserializeList(
		is,
		type,		// Identifies the object type linked to this inventory
		_hash       // Hash of the object
	);
	_type = static_cast<Type>(type);
}

SVal protocol::InventoryVector::toSVal() const
{
	SObj obj;
	obj.emplace("type",
		_type == Type::MSG_TX ? "Tx" :
		_type == Type::MSG_BLOCK ? "Block" :
		_type == Type::MSG_FILTERED_BLOCK ? "FilteredBlock" :
		_type == Type::MSG_CMPCT_BLOCK ? "CompactBlock" :
		"Error"
	);
	obj.emplace("hash", _hash.toSVal());
	return obj;
}
