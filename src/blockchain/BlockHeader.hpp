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
//


// BlockHeader.hpp

#pragma once

#include <cstdint>
#include <cassert>

#include "../types/Blobs.hpp"
#include "../serialization/Serialization.hpp"

class BlockHeader : public Serializable
{
	uint32_t _version = 0;
	uint256 _prev;
	uint256 _merkle;
	uint32_t _timestamp = 0;
	uint32_t _bits = 0;
	uint32_t _nonce = 0;
	uint32_t _chain = -1;

	size_t _id = -1;
	size_t _height = -1;
	mutable uint256 _hash;
	mutable bool _validHash = 0;

public:
	BlockHeader() = default; // Default-constructor
	BlockHeader(BlockHeader&&) noexcept = default; // Move-constructor
	BlockHeader(const BlockHeader&) = default; // Copy-constructor
	~BlockHeader() = default; // Destructor
	BlockHeader& operator=(BlockHeader&&) noexcept = default; // Move-assignment
	BlockHeader& operator=(BlockHeader const&) = default; // Copy-assignment

	void reset()
	{
		_version = 0;
		_prev.setNull();
		_merkle.setNull();
		_timestamp = 0;
		_bits = 0;
		_nonce = 0;
		_chain = -1;

		_id = -1;
		_height = -1;
		_hash.setNull();
		_validHash = -1;
	}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;

	[[nodiscard]]
	const uint256& hash() const;

	[[nodiscard]]
	const uint256& prev() const
	{
		return _prev;
	}

	[[nodiscard]]
	const uint256& merkle() const
	{
		return _merkle;
	}

	[[nodiscard]]
	size_t id() const
	{
		return _id;
	}

	void setId(size_t id)
	{
		assert(_id == -1);
		_id = id;
	}

	[[nodiscard]]
	size_t height() const
	{
		return _height;
	}

	void setHeight(size_t height)
	{
		assert(_height == -1);
		_height = height;
	}

	[[nodiscard]]
	size_t inChain() const
	{
		return _height != -1;
	}


};


