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


// MessageHeader.hpp

#pragma once


#include <serialization/Serialization.hpp>
#include <types/Blobs.hpp>

namespace protocol
{

class MessageHeader final : public Serializable
{
public:
	static constexpr size_t MESSAGE_START_SIZE = 4;
	static constexpr size_t COMMAND_SIZE = 12;
	static constexpr size_t MESSAGE_SIZE_SIZE = 4;
	static constexpr size_t CHECKSUM_SIZE = 4;
	static constexpr size_t MESSAGE_SIZE_OFFSET = MESSAGE_START_SIZE + COMMAND_SIZE;
	static constexpr size_t CHECKSUM_OFFSET = MESSAGE_SIZE_OFFSET + MESSAGE_SIZE_SIZE;
	static constexpr size_t HEADER_SIZE = MESSAGE_START_SIZE + COMMAND_SIZE + MESSAGE_SIZE_SIZE + CHECKSUM_SIZE;
	typedef unsigned char MessageStartChars[MESSAGE_START_SIZE];

private:
	mutable uint32_t _magic;                    // Magic value indicating messages origin network, and used to seek to next messages when stream state is unknown
	mutable std::array<uint8_t, 12> _command;   // ASCII string identifying the messages content, NULL padded (non-NULL padding results in messages rejected)
	mutable uint32_t _length;                   // Length of payload in number of bytes
	mutable uint32_t _checksum;                 // First 4 bytes of sha256(sha256(payload))

public:
	MessageHeader(MessageHeader&&) noexcept = delete; // Move-constructor
	MessageHeader(const MessageHeader&) = delete; // Copy-constructor
	~MessageHeader() = default; // Destructor
	MessageHeader& operator=(MessageHeader&&) noexcept = default; // Move-assignment
	MessageHeader& operator=(MessageHeader const&) = delete; // Copy-assignment

	MessageHeader() // Default-constructor
	: _magic(0)
	, _command()
	, _length(0)
	, _checksum(0)
	{
	}

	MessageHeader(
		uint32_t magic,
		std::string_view command,
		size_t length,
		uint32_t checksum
	)
	: _magic(magic)
	, _length(length)
	, _checksum(checksum)
	{
		auto i = std::copy(command.begin(), command.end(), _command.begin());
		std::fill(i, _command.end(), 0);
	}

	void Serialize(std::ostream& os) const override;
	void Unserialize(std::istream& is) override;

	[[nodiscard]]
	SVal toSVal() const override;

	uint32_t magic() const
	{
		return _magic;
	}

	std::string command() const
	{
		std::string ret;
		std::copy_if(_command.begin(), _command.end(), std::inserter(ret, ret.begin()), [](auto c){ return c; });
		return ret;
	}

	uint32_t length() const
	{
		return _length;
	}

	uint32_t checksum() const
	{
		return _checksum;
	}
};

}
