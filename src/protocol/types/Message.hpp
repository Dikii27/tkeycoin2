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


// Message.hpp

#pragma once

#include <cstdint>
#include <string>
#include <protocol/types/Magic.hpp>
#include <vector>
#include <serialization/Serialization.hpp>
#include <other/HashStream.hpp>
#include <protocol/types/MessageHeader.hpp>
#include <cstring>
#include <crypto/sha256.h>
#include <node/Node.hpp>

namespace protocol
{

class Message : public Serializable
{
private:
	enum class Direct
	{
		Undefined,
		Inner,
		Outer
	};
	Direct _direct = Direct::Undefined;

public:
	Message() = default; // Default-constructor
	Message(Message&&) noexcept = default; // Move-constructor
	Message(const Message&) = delete; // Copy-constructor
	virtual ~Message() = default; // Destructor
	Message& operator=(Message&&) noexcept = delete; // Move-assignment
	Message& operator=(Message const&) = delete; // Copy-assignment

	[[nodiscard]]
	virtual const std::string& command() const = 0;

	[[nodiscard]]
	MessageHeader buildHeader() const;

	virtual void apply(
		const std::shared_ptr<Node>& node,
		const std::shared_ptr<Peer>& peer
	) const = 0;
};

}

#include <protocol/MessageFactory.hpp>

#define REGISTER_MESSAGE(MessageName)                                                           \
const int protocol::message::MessageName::__dummy = (protocol::message::MessageName::__classId(), 0); \

#define DECLARE_MESSAGE(MessageName) \
public:                                                                                         \
    [[nodiscard]]                                                                               \
    static const std::string& Command()                                                         \
    {                                                                                           \
        static const std::string command = []{                                                  \
            std::string name(#MessageName);                                                     \
            std::for_each(std::begin(name), end(name), [](auto& c){c=std::tolower(c);});        \
            return name;                                                                        \
		}();                                                                                    \
        return command;                                                                         \
    }                                                                                           \
                                                                                                \
    [[nodiscard]]                                                                               \
    const std::string& command() const override                                                 \
    {                                                                                           \
        return Command();                                                                       \
    }                                                                                           \
                                                                                                \
    MessageName(MessageName&&) noexcept = default;                                              \
    MessageName(const MessageName&) = delete;                                                   \
    ~MessageName() override = default;                                                          \
    MessageName& operator=(MessageName&&) noexcept = delete;                                    \
    MessageName& operator=(const MessageName&) = delete;                                        \
                                                                                                \
	void Serialize(std::ostream& os) const override;                                            \
	void Unserialize(std::istream& is) override;                                                \
                                                                                                \
	[[nodiscard]]                                                                               \
	SVal toSVal() const override;                                                               \
                                                                                                \
	void apply(                                                                                 \
		const std::shared_ptr<Node>& node,                                                      \
		const std::shared_ptr<Peer>& peer                                                       \
	) const override;                                                                           \
                                                                                                \
private:                                                                                        \
    static std::shared_ptr<Message> create()                                                    \
    {                                                                                           \
        return std::make_shared<MessageName>();                                                 \
    }                                                                                           \
                                                                                                \
public:                                                                                         \
	static size_t __classId()                                                                   \
	{                                                                                           \
		std::cout << "auto registration: " << Command() << std::endl;                           \
	    MessageFactory::reg(                                                                    \
	        protocol::message::MessageName::Command(),                                          \
	        protocol::message::MessageName::create                                              \
	    );                                                                                      \
		return (size_t)&__classId;                                                              \
	}                                                                                           \
                                                                                                \
private:                                                                                        \
	[[maybe_unused]]                                                                            \
	static const int __dummy;


