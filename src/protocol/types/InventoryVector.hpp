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


// InventoryVector.hpp

#pragma once


#include <cstdint>
#include <types/Blobs.hpp>

namespace protocol
{

class InventoryVector final : public Serializable
{
public:
	enum class Type : uint32_t
	{
		ERROR = 0,              // Any data of with this number may be ignored
		MSG_TX = 1,             // Hash is related to a transaction
		MSG_BLOCK = 2,          // Hash is related to a data block
		MSG_FILTERED_BLOCK = 3, // Hash of a block header; identical to MSG_BLOCK. Only to be used in getdata messages. Indicates the reply should be a merkleblock messages rather than a block messages; this only works if a bloom filter has been set.
		MSG_CMPCT_BLOCK = 4,    // Hash of a block header; identical to MSG_BLOCK. Only to be used in getdata messages. Indicates the reply should be a cmpctblock messages. See BIP 152 for more info.
	};

private:
	Type _type = Type::ERROR;   // Identifies the object type linked to this inventory
	uint256 _hash;              // Hash of the object

public:
	InventoryVector() = default; // Default-constructor
	InventoryVector(InventoryVector&&) noexcept = default; // Move-constructor
	InventoryVector(const InventoryVector&) = default; // Copy-constructor
	~InventoryVector() = default; // Destructor
	InventoryVector& operator=(InventoryVector&&) noexcept = default; // Move-assignment
	InventoryVector& operator=(InventoryVector const&) = default; // Copy-assignment

	InventoryVector(Type type, uint256 hash)
	: _type(type)
	, _hash(std::move(hash))
	{
	}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;

	[[nodiscard]]
	InventoryVector::Type type() const
	{
		return _type;
	}

	[[nodiscard]]
	const uint256& hash() const
	{
		return _hash;
	}

	bool operator==(const InventoryVector& other) const
	{
		return _type == other._type && _hash == other._hash;
	}
};

}

namespace std
{
	template<>
	class hash<protocol::InventoryVector>
	{
		public:
		size_t operator()(const protocol::InventoryVector& item) const
		{
//			return std::_Hash_bytes(blob.data(), blob.size());
			return *reinterpret_cast<const size_t*>(item.hash().data());
		}
	};
}
