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
#include <cstdint>
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
	static constexpr bool Apply(const From& /*src*/, To& /*dest*/)
	{
		return false;
	}
};

// ConvertAssign<std::basic_string<wchar_t>, std::basic_string<char>> - Convert from wide string to byte string
template<class AllocN, class AllocW>
struct Epic::EON::Convert<std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocW>, std::basic_string<char, std::char_traits<char>, AllocN>>
{
	static inline bool Apply(const std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocW>& src, 
								std::basic_string<char, std::char_traits<char>, AllocN>& dest)
	{
		// Convert a wide string to a string
		using CodeCVT = std::codecvt_utf8<wchar_t>;
		using Converter = std::wstring_convert<CodeCVT, wchar_t, AllocW, AllocN>;

		dest = Converter().to_bytes(src);

		return true;
	}
};

// ConvertAssign<std::basic_string<char>, std::basic_string<wchar_t>> - Convert from byte string to wide string
template<class AllocN, class AllocW>
struct Epic::EON::Convert<std::basic_string<char, std::char_traits<char>, AllocN>, std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocW>>
{
	static inline bool Apply(const std::basic_string<char, std::char_traits<char>, AllocN>& src,
							 std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocW>& dest)
	{
		// Convert a narrow string to a wide string
		using CodeCVT = std::codecvt_utf8<wchar_t>;
		using Converter = std::wstring_convert<CodeCVT, wchar_t, AllocW, AllocN>;

		dest = Converter().from_bytes(src);

		return true;
	}
};

// ConvertAssign<Epic::EON::String, std::basic_string> - Convert from EON::String to std::string
template<class Char, class Alloc>
struct Epic::EON::Convert<Epic::EON::String, std::basic_string<Char, std::char_traits<Char>, Alloc>>
{
	static inline bool Apply(const Epic::EON::String& src,
							 std::basic_string<Char, std::char_traits<Char>, Alloc>& dest)
	{
		return Epic::EON::Convert<std::decay_t<decltype(src.Value)>, std::decay_t<decltype(dest)>>::Apply(src.Value, dest);
	}
};

// ConvertAssign<std::basic_string, Epic::BasicStringHash> - Convert from std::basic_string to Epic::BasicStringHash
template<class CharS, class CharH, class Alloc, Epic::StringHashAlgorithms Algorithm>
struct Epic::EON::Convert<std::basic_string<CharS, std::char_traits<CharS>, Alloc>, Epic::BasicStringHash<CharH, Algorithm>>
{
	static inline bool Apply(const std::basic_string<CharS, std::char_traits<CharS>, Alloc>& src,
							 Epic::BasicStringHash<CharH, Algorithm>& dest)
	{
		dest = Epic::Hash(src);
		return true;
	}
};

// ConvertAssign<Epic::EON::String, Epic::BasicStringHash> - Convert from EON::String to Epic::BasicStringHash
template<class CharH, Epic::StringHashAlgorithms Algorithm>
struct Epic::EON::Convert<Epic::EON::String, Epic::BasicStringHash<CharH, Algorithm>>
{
	static inline bool Apply(const Epic::EON::String& src, Epic::BasicStringHash<CharH, Algorithm>& dest)
	{
		return Epic::EON::Convert<std::decay_t<decltype(src.Value)>, std::decay_t<decltype(dest)>>::Apply(src.Value, dest);
	}
};
