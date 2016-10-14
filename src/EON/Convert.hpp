//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2016 Ronnie Brohn (EpicBrownie)      
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

#include <Epic/EON/Types.hpp>
#include <Epic/StringHash.hpp>
#include <codecvt>
#include <locale>
#include <string>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	template<class From, class To>
	struct Convert;
}

// ConvertAssign<From, To> - Convert from type From to type To
template<class From, class To>
struct Epic::EON::Convert
{
	bool operator() (const From& /*src*/, To& /*dest*/) const
	{
		return false;
	}
};

// ConvertAssign<EON::String, std::basic_string<char>> - Convert from EON::String (wide) to narrow string
template<class Alloc>
struct Epic::EON::Convert<Epic::EON::String, std::basic_string<char, std::char_traits<char>, Alloc>>
{
	bool operator() (const Epic::EON::String& src, std::basic_string<char, std::char_traits<char>, Alloc>& dest) const
	{
		// Convert a wide string to a string
		using CodeCVT = std::codecvt_utf8<wchar_t>;
		using Converter = std::wstring_convert<CodeCVT, wchar_t, Epic::EON::String::ValueType::allocator_type, Alloc>;

		dest = Converter().to_bytes(src.Value);

		return true;
	}
};

// ConvertAssign<std::basic_string<char>, std::basic_string<wchar_t>> - Convert from narrow string to wide string
template<class AllocN, class AllocW>
struct Epic::EON::Convert<std::basic_string<char, std::char_traits<char>, AllocN>, std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocW>>
{
	bool operator() (const std::basic_string<char, std::char_traits<char>, AllocN>& src,
					 std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocW>& dest) const
	{
		// Convert a narrow string to a wide string
		using CodeCVT = std::codecvt_utf8<wchar_t>;
		using Converter = std::wstring_convert<CodeCVT, wchar_t, AllocW, AllocN>;

		dest = Converter().from_bytes(src);

		return true;
	}
};

// ConvertAssign<EON::String, Epic::BasicStringHash> - Convert from EON::String to Epic::BasicStringHash
template<Epic::StringHashAlgorithms Algorithm>
struct Epic::EON::Convert<Epic::EON::String, Epic::BasicStringHash<Epic::EON::String::ValueType::value_type, Algorithm>>
{
	bool operator() (const Epic::EON::String& src,
					 Epic::BasicStringHash<Epic::EON::String::ValueType::value_type, Algorithm>& dest) const
	{
		dest = src.Value;
		return true;
	}
};
