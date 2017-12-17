//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2017 Ronnie Brohn (EpicBrownie)      
//
//                Distributed under The MIT License (MIT).
//             (See accompanying file License.txt or copy at 
//                 https://opensource.org/licenses/MIT)
//
//           Please report any bugs, typos, or suggestions to
//              https://github.com/epicbrownie/Epic/issues
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Epic/StringHash.hpp>
#include <Epic/Math/Matrix.hpp>
#include <Epic/Math/Vector.hpp>
#include <Epic/Math/Quaternion.hpp>
#include <Epic/TMP/TypeTraits.hpp>
#include <cwchar>
#include <codecvt>
#include <clocale>
#include <sstream>
#include <string>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	template<class F, class T>
	struct TypeConvert;
}

//////////////////////////////////////////////////////////////////////////////

template<class F, class T>
struct Epic::EON::detail::TypeConvert
{
public:
	template<class C, class Tr, class A, class T>
	using IsStreamableT = decltype(std::declval<std::basic_istringstream<C, Tr, A>>().operator>>(std::declval<T&>()));

public:
	// string -> T (via stringstream conversion)
	template<typename C, class Tr, class A,
			 typename = std::enable_if_t<Epic::TMP::IsDetected<IsStreamableT, C, Tr, A, T>::value>>
	bool operator() (T& to, const std::basic_string<C, Tr, A>& from)
	{
		std::basic_istringstream<C, Tr, A>(from) >> to;
		return true;
	}

	// string -> wstring (via wstring_convert)
	template<class AllocB, class AllocW>
	bool operator() (std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocW>& to,
					 const std::basic_string<char, std::char_traits<char>, AllocB>& from)
	{
		// Convert a byte string to a wide string
		using CodeCVT = std::codecvt_utf8<wchar_t>;
		using Converter = std::wstring_convert<CodeCVT, wchar_t, AllocW, AllocB>;

		to = Converter().from_bytes(from);

		return true;
	};

	// wstring -> string (via wstring_convert)
	template<class AllocB, class AllocW>
	bool operator() (std::basic_string<char, std::char_traits<char>, AllocB>& to,
					 const std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocW>& from)
	{
		// Convert a wide string to a string
		using CodeCVT = std::codecvt_utf8<wchar_t>;
		using Converter = std::wstring_convert<CodeCVT, wchar_t, AllocW, AllocB>;

		to = Converter().to_bytes(from);

		return true;
	}

	// string -> BasicStringHash
	template<typename CS, class Tr, class A, class CH, Epic::StringHashAlgorithms Alg>
	bool operator() (Epic::BasicStringHash<CH, Alg>& to, const std::basic_string<CS, Tr, A>& from)
	{
		to = Epic::Hash(from);
		return true;
	}

	// bool -> string
	template<typename C, class Tr, class A>
	bool operator() (std::basic_string<C, Tr, A>& to, const bool& from)
	{
		to = from ? C('1') : C('0');
		return true;
	}

	// long -> string
	template<typename C, class Tr, class A>
	bool operator() (std::basic_string<C, Tr, A>& to, const long& from)
	{
		C buff[21]; // [-2^63 and 2^64 - 1] + '\0'
		C* const pEnd = std::end(buff);
		C* pNext = pEnd;

		auto ufrom = static_cast<unsigned long>(from);
		if (from < 0)
		{
			pNext = std::_UIntegral_to_buff(pNext, 0 - ufrom);
			*--pNext = '-';
		}
		else
			pNext = std::_UIntegral_to_buff(pNext, ufrom);

		to.assign(pNext, pEnd);

		return true;
	}

	// double -> string
	template<typename C, class Tr, class A>
	bool operator() (std::basic_string<C, Tr, A>& to, const double& from)
	{
		std::basic_ostringstream<C, Tr, A> ss;
		ss << from;
		to = ss.str();

		return true;
	}

	// size_t -> string
	template<typename C, class Tr, class A>
	bool operator() (std::basic_string<C, Tr, A>& to, const size_t& from)
	{
		std::basic_ostringstream<C, Tr, A> ss;
		ss << from;
		to = ss.str();

		return true;
	}

	// string -> VectorN
	template<class C, class Tr, class A, class T, size_t Sz>
	bool operator() (Epic::Vector<T, Sz>& to, const std::basic_string<C, Tr, A>& from)
	{
		constexpr Epic::StringHash HOne = "One";
		constexpr Epic::StringHash HZero = "Zero";
		constexpr Epic::StringHash HIdentity = "Identity";

		switch (Epic::Hash(from))
		{
			case HOne: 
				to = Epic::One; 
				return true;
			
			case HZero: 
				to = Epic::Zero; 
				return true;
			
			case HIdentity: 
				to = Epic::Identity; 
				return true;
			
			default: 
				return false;
		}
	}

	// string -> Quaternion
	template<class C, class Tr, class A, class T>
	bool operator() (Epic::Quaternion<T>& to, const std::basic_string<C, Tr, A>& from)
	{
		constexpr Epic::StringHash HIdentity = "Identity";

		if (Epic::Hash(from) == HIdentity)
		{
			to = Epic::Identity;
			return true;
		}

		return false;
	}

	// string -> Matrix
	template<class C, class Tr, class A, class T, size_t Sz>
	bool operator() (Epic::Matrix<T, Sz>& to, const std::basic_string<C, Tr, A>& from)
	{
		constexpr Epic::StringHash HOne = "One";
		constexpr Epic::StringHash HZero = "Zero";
		constexpr Epic::StringHash HIdentity = "Identity";

		switch (Epic::Hash(from))
		{
			case HOne: 
				to = Epic::One; 
				return true;
			
			case HZero: 
				to = Epic::Zero; 
				return true;
			
			case HIdentity: 
				to = Epic::Identity; 
				return true;
			
			default: 
				return false;
		}
	}
};
