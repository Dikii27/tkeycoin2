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


// Serialization.hpp

#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <sstream>
#include <serialization/SVal.hpp>

class Serializable
{
public:
	virtual void Serialize(std::ostream& os) const = 0;
	virtual void Unserialize(std::istream& is) = 0;

	[[nodiscard]]
	virtual SVal toSVal() const = 0;
};

// Helpers
template<typename T,typename> void Serialize(std::ostream&, const T&);
template<typename T,typename> void Serialize(std::ostream&, T&&);
template<typename T,typename> void Unserialize(std::istream&, T&);


template<class T>
struct is_smart_ptr : std::false_type {};

template<class T>
struct is_smart_ptr<std::shared_ptr<T>> : std::true_type {};

template<class T>
struct is_smart_ptr<std::unique_ptr<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_smart_ptr_v = is_smart_ptr<T>::value;


template<typename T, bool = std::is_base_of_v<Serializable, T>>
struct __is_serializable_by_method_helper
{ typedef typename std::true_type type; };

template<typename T>
struct __is_serializable_by_method_helper<T, false>
{ typedef typename std::false_type type; };

template<typename T>
struct is_serializable_by_method : public __is_serializable_by_method_helper<T>::type
{ };

template<typename T>
inline constexpr bool is_serializable_by_method_v = is_serializable_by_method<T>::value;


// For serializable by method

template<typename T, typename std::enable_if_t<is_serializable_by_method_v<T>, void>* = nullptr>
void Serialize(std::ostream& os, const T& value)
{
	value.Serialize(os);
}

template<typename T, typename std::enable_if_t<is_serializable_by_method_v<T>, void>* = nullptr>
void Unserialize(std::istream& is, T& reference)
{
	reference.Unserialize(is);
}

template<typename T, typename std::enable_if_t<is_serializable_by_method_v<T>, void>* = nullptr>
void Unserialize(std::istream& is, T&& reference)
{
	reference.Unserialize(is);
}


// For integral types

template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
void Serialize(std::ostream& os, const T& value)
{
	os.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void>* = nullptr>
void Unserialize(std::istream& is, T& reference)
{
	is.read(reinterpret_cast<char*>(&reference), sizeof(T));
}


// For various string views

inline void Serialize(std::ostream& os, std::basic_string_view<char> view)
{
	os.write(reinterpret_cast<const char*>(view.data()), view.length());
}

inline void Unserialize(std::istream& is, std::string& str)
{
	is.read(reinterpret_cast<char*>(str.data()), str.length());
}


// For smart pointers

template<typename T, typename std::enable_if_t<is_serializable_by_method_v<T>, void>* = nullptr>
void Serialize(std::ostream& os, const std::shared_ptr<T>& value)
{
	value->Serialize(os);
}

template<typename T, typename std::enable_if_t<is_serializable_by_method_v<T>, void>* = nullptr>
void Unserialize(std::istream& is, std::shared_ptr<T>& reference)
{
	if (!reference)
	{
		reference = std::make_shared<T>();
	}
	reference->Unserialize(is);
}

template<typename T, typename std::enable_if_t<!is_serializable_by_method_v<T>, void>* = nullptr>
void Serialize(std::ostream& os, const std::shared_ptr<T>& value)
{
	::Serialize(os, *value);
}

template<typename T, typename std::enable_if_t<!is_serializable_by_method_v<T>, void>* = nullptr>
void Unserialize(std::istream& is, std::shared_ptr<T>& reference)
{
	if (!reference)
	{
		reference = std::make_shared<T>();
	}
	::Unserialize(is, *reference);
}


// For vectors of serializable

template<typename T, typename std::enable_if_t<is_serializable_by_method_v<T>, void>* = nullptr>
void Serialize(std::ostream& os, const std::vector<T>& vec)
{
	std::for_each(vec.cbegin(), vec.cend(), [&os](const auto& value){
		value.Serialize(os);
	});
}

template<typename T, typename std::enable_if_t<is_serializable_by_method_v<T>, void>* = nullptr>
void Unserialize(std::istream& is, std::vector<T>& vec)
{
	std::for_each(vec.begin(), vec.end(), [&is](auto& value){
		value.Unserialize(is);
	});
}


// For vector of integral types

template<typename T, typename std::enable_if_t<!is_serializable_by_method_v<T> && std::is_integral_v<T>, void>* = nullptr>
void Serialize(std::ostream& os, const std::vector<T>& vec)
{
	os.write(reinterpret_cast<const char*>(vec.data()), sizeof(T[vec.size()]));
}

template<typename T, typename std::enable_if_t<!is_serializable_by_method_v<T> && std::is_integral_v<T>, void>* = nullptr>
void Unserialize(std::istream& is, std::vector<T>& vec)
{
	is.read(reinterpret_cast<char*>(vec.data()), sizeof(T[vec.size()]));
}


// For array of integral types

template<typename T, size_t N, typename std::enable_if_t<!is_serializable_by_method_v<T> && std::is_integral_v<T>, void>* = nullptr>
void Serialize(std::ostream& os, const std::array<T,N>& vec)
{
	os.write(reinterpret_cast<const char*>(vec.data()), sizeof(T[vec.size()]));
}

template<typename T, size_t N, typename std::enable_if_t<!is_serializable_by_method_v<T> && std::is_integral_v<T>, void>* = nullptr>
void Unserialize(std::istream& is, std::array<T, N>& vec)
{
	is.read(reinterpret_cast<char*>(vec.data()), sizeof(T[vec.size()]));
}


// For vector of other types

template<typename T, typename std::enable_if_t<!is_serializable_by_method_v<T> && !std::is_integral_v<T>, void>* = nullptr>
void Serialize(std::ostream& os, const std::vector<T>& vec)
{
	std::for_each(vec.cbegin(), vec.cend(), [&os](const auto& value){
		Serialize(os, value);
	});
}

template<typename T, typename std::enable_if_t<!is_serializable_by_method_v<T> && !std::is_integral_v<T>, void>* = nullptr>
void Unserialize(std::istream& is, std::vector<T>& vec)
{
	std::for_each(vec.begin(), vec.end(), [&is](auto& value){
		Unserialize(is, value);
	});
}


// Recursive call for argument list

template<typename T, typename... Args, typename std::enable_if<(sizeof...(Args) > 0), void>* = nullptr >
void SerializeList(std::ostream& os, const T& value, const Args&...args)
{
	Serialize(os, value);
	if constexpr (sizeof...(args)>0)
	{
		SerializeList(os, args...);
	}
}

template<typename T, typename... Args, typename std::enable_if<(sizeof...(Args) > 0), void>* = nullptr >
void UnserializeList(std::istream& is, const T& reference, Args&&...args)
{
	Unserialize(is, reference);
	if constexpr (sizeof...(args)>0)
	{
		UnserializeList(is, std::forward<Args>(args)...);
	}
}

template<typename T, typename... Args, typename std::enable_if<(sizeof...(Args) > 0), void>* = nullptr >
void UnserializeList(std::istream& is, T&& reference, Args&&...args)
{
	Unserialize(is, std::forward<T>(reference));
	if constexpr (sizeof...(args)>0)
	{
		UnserializeList(is, std::forward<Args>(args)...);
	}
}


//template<typename T>
//class __is_serializable_by_func_helper
//{
//	static void detect()
//	{
//		std::stringstream ss;
//		T t;
//		::Serialize(ss, t);
//		::Unserialize(ss, t);
//	}
//
//public:
//	typedef typename std::conditional_t<std::is_same_v<void, typename std::decay_t<decltype(detect())>>,
//		std::true_type,
//		std::false_type> type;
//};




//constexpr bool detect()
//{
//	return false;
//}
//
//template<typename T>
//constexpr bool detect()
//{
//	std::stringstream* ss = nullptr;
//	T t;
//	::Serialize<T>(*ss, t);
//	::Unserialize<T>(*ss, t);
//	return true;
//}
//
//
//template<typename T>
//class __is_serializable_by_func_helper
//	: public std::conditional_t<
//		detect<T>(),
//		std::true_type,
//		std::false_type
//	>
//{
//};
//
//static_assert( std::is_invocable<Serialize(std::ostream, )>::value );




//
//template<typename>
//struct is_serializable_by_func : public std::false_type { };
//
//template<typename T>
//struct is_serializable_by_func<__is_serializable_by_func_helper<T>> : public std::true_type { };
//
//template<typename T, typename S>
//inline constexpr bool is_serializable_by_func_v = is_serializable_by_func<T>::value;


//template<typename T, typename _ = void>
//struct is_container : std::false_type {};
//
//template<typename... Ts>
//struct is_container_helper {};
//
//template<typename T>
//struct is_container<
//	T,
//	std::conditional_t<
//		false,
//		is_container_helper<
//			typename T::value_type,
//			typename T::size_type,
//			typename T::allocator_type,
//			typename T::iterator,
//			typename T::const_iterator,
//			decltype(std::declval<T>().size()),
//			decltype(std::declval<T>().begin()),
//			decltype(std::declval<T>().end()),
//			decltype(std::declval<T>().cbegin()),
//			decltype(std::declval<T>().cend())
//		>,
//		void
//	>
//> : public std::true_type {};
//
//
//template<typename T>
//inline constexpr bool is_serializable_by_func1 =
//	std::is_integral_v<T> || is_serializable_by_method_v<T>;
//
//template<typename T>
//inline constexpr bool is_serializable_smart_ptr =
//	is_smart_ptr_v<T> && is_serializable_by_func1<T::element_type>;
//
//
//template<typename T>
//inline constexpr bool is_serializable_container =
//	is_container<T>::value && is_serializable_by_func1<T::value_type>;
//
//template<typename T>
//inline constexpr bool is_serializable =
//	std::is_integral_v<T> || is_serializable_by_method_v<T> || is_serializable_smart_ptr<T> || is_serializable_container<T>;
