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


// SerHelper.hpp

#pragma once

#include "Serialization.hpp"
#include "../types/VariableLengthInteger.hpp"

template<typename T>
class SerializationWrapper : public Serializable
{
private:
	const T& _ref;
	const bool _const;

public:
	SerializationWrapper() = delete; // Default-constructor
	SerializationWrapper(SerializationWrapper&&) noexcept = delete; // Move-constructor
	SerializationWrapper(const SerializationWrapper&) = delete; // Copy-constructor
	~SerializationWrapper() = default; // Destructor
	SerializationWrapper& operator=(SerializationWrapper&&) noexcept = delete; // Move-assignment
	SerializationWrapper& operator=(SerializationWrapper const&) = delete; // Copy-assignment

	explicit SerializationWrapper(const std::remove_cv_t<T>& orig) : _ref(orig), _const(true) {}

	explicit SerializationWrapper(std::remove_cv_t<T>&& orig) : _ref(orig), _const(false) {}

	explicit SerializationWrapper(std::remove_cv_t<T>& orig) : _ref(orig), _const(false) {}

	void Serialize(std::ostream& os) const override
	{
		::Serialize(os, uintV(_ref.size()));
		::Serialize(os, _ref);
	}

	void Unserialize(std::istream& is) override
	{
		auto& ref = const_cast<std::remove_cv_t<T>&>(_ref);

		uintV size;
		::Unserialize(is, size);

		ref.resize(size);
		::Unserialize(is, ref);
	}

	[[nodiscard]]
	SVal toSVal() const override { throw std::runtime_error("This is code must never calling"); }
};

template<class T>
inline SerializationWrapper<T> size_and_(T& data)
{
	return SerializationWrapper<T>(data);
}
