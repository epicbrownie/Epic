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

#include <Epic/StringHashAlgorithm.hpp>
#include <cstdint>
#include <functional>
#include <string>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<typename CharType = char,
			 Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	class StringHash;
}

//////////////////////////////////////////////////////////////////////////////

// StringHash
template<typename C, Epic::StringHashAlgorithms A>
class Epic::StringHash
{
public:
	using CharType = C;
	static constexpr Epic::StringHashAlgorithms Algorithm = A;

public:
	struct CStringWrapper
	{
		CStringWrapper(const CharType* cstr)
			: Str(cstr)
		{
			/* Non-explicit C'tor important here */
		};

		const CharType* Str;
	};

public:
	template<size_t N>
	explicit constexpr StringHash(const CharType(&cstr)[N]) noexcept
		: _Hash{ Epic::StringHashAlgorithm<C, A>::FoldHash(cstr) } { }

	explicit inline StringHash(CStringWrapper cstr) noexcept
		: _Hash{ Epic::StringHashAlgorithm<C, A>::Hash(cstr.Str) } { }

	template<typename traits_type, typename allocator_type>
	explicit inline StringHash(const std::basic_string<C, traits_type, allocator_type>& str) noexcept
		: _Hash{ Epic::StringHashAlgorithm<C, A>::Hash(str.c_str()) } { }

public:
	inline constexpr operator size_t (void) const noexcept
	{
		return _Hash;
	}

public:
	#define CREATE_COMPARISON_OPERATOR(Op)	\
		inline constexpr bool operator Op (const Epic::StringHash<C, A>& rhs) const noexcept					\
		{																										\
			return _Hash Op rhs._Hash;																		\
		}

	CREATE_COMPARISON_OPERATOR(== );
	CREATE_COMPARISON_OPERATOR(!= );
	CREATE_COMPARISON_OPERATOR(<);
	CREATE_COMPARISON_OPERATOR(>);
	CREATE_COMPARISON_OPERATOR(<=);
	CREATE_COMPARISON_OPERATOR(>=);

public:
	friend struct std::hash<Epic::StringHash<C, A>>;

private:
	size_t _Hash;
};

//////////////////////////////////////////////////////////////////////////////

// UDLs
namespace Epic
{
	namespace UDL
	{
		// String Hash UDLs
		inline Epic::StringHash<char> operator "" _hash(const char* str, size_t len)
		{
			return Epic::StringHash<char>(str);
		}

		inline Epic::StringHash<wchar_t> operator "" _hash(const wchar_t* str, size_t len)
		{
			return Epic::StringHash<wchar_t>(str);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// std::hash
namespace std
{
	template<typename C, Epic::StringHashAlgorithms A>
	struct hash<Epic::StringHash<C, A>>
	{
		size_t operator() (const Epic::StringHash<C, A>& x) const noexcept
		{
			return x._Hash;
		}
	};
}

