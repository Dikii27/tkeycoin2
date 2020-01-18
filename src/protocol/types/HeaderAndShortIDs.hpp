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


// HeaderAndShortIDs.hpp

#pragma once


#include <blockchain/BlockHeader.hpp>
#include "PrefilledTransaction.hpp"

namespace protocol
{

class HeaderAndShortIDs final : public Serializable
{
private:
	// First 80 bytes of the block as defined by the encoding used by "block" messages	The header of the block being provided
	std::shared_ptr<BlockHeader> _header;

	// A nonce for use in short transaction ID calculations
	uint64_t _nonce;

	// List of 6-byte integers	6*shortids_length bytes	Little Endian	The short transaction IDs calculated from the transactions which were not provided explicitly in prefilledtxn
	std::vector<uint48> _shortIds;

	// List of PrefilledTransactions	variable size*prefilledtxn_length	As defined by PrefilledTransaction definition, above	Used to provide the coinbase transaction and a select few which we expect a peer may be missing
	std::vector<protocol::PrefilledTransaction> _prefilledTxn;

public:
	HeaderAndShortIDs() = default; // Default-constructor
	HeaderAndShortIDs(HeaderAndShortIDs&&) noexcept = default; // Move-constructor
	HeaderAndShortIDs(const HeaderAndShortIDs&) = delete; // Copy-constructor
	~HeaderAndShortIDs() = default; // Destructor
	HeaderAndShortIDs& operator=(HeaderAndShortIDs&&) noexcept = delete; // Move-assignment
	HeaderAndShortIDs& operator=(HeaderAndShortIDs const&) = delete; // Copy-assignment

	HeaderAndShortIDs(
		std::shared_ptr<BlockHeader> header,
		uint64_t nonce,
		std::vector<uint48> shortIds,
		std::vector<protocol::PrefilledTransaction> prefilledTxn
	)
	: _header(std::move(header))
	, _nonce(nonce)
	, _shortIds(std::move(shortIds))
	, _prefilledTxn(std::move(prefilledTxn))
	{}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;
};
}
