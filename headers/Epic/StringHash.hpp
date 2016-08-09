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
	template<typename CharType = char, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	class StringHash;
}

//////////////////////////////////////////////////////////////////////////////

/// StringHash<C, A>
template<typename C, Epic::StringHashAlgorithms A>
class Epic::StringHash
{
public:
	using CharType = C;
	using AlgorithmType = Epic::StringHashAlgorithm<C, A>;
	using HashType = typename AlgorithmType::HashType;

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

	template<typename Traits, typename Allocator>
	explicit inline StringHash(const std::basic_string<C, Traits, Allocator>& str) noexcept
		: _Hash{ Epic::StringHashAlgorithm<C, A>::Hash(str.c_str()) } { }

public:
	inline constexpr operator HashType (void) const noexcept
	{
		return _Hash;
	}

public:
	constexpr HashType Value() const noexcept
	{
		return _Hash;
	}

public:
	#define CREATE_COMPARISON_OPERATOR(Op)	\
		inline constexpr bool operator Op (const Epic::StringHash<C, A>& rhs) const noexcept	\
		{																						\
			return _Hash Op rhs._Hash;															\
		}

	CREATE_COMPARISON_OPERATOR(== );
	CREATE_COMPARISON_OPERATOR(!= );
	CREATE_COMPARISON_OPERATOR(<);
	CREATE_COMPARISON_OPERATOR(>);
	CREATE_COMPARISON_OPERATOR(<=);
	CREATE_COMPARISON_OPERATOR(>=);

	#undef CREATE_COMPARISON_OPERATOR

private:
	const HashType _Hash;
};

//////////////////////////////////////////////////////////////////////////////

/// Shortcuts
namespace Epic
{
	/// StringHash<N, CharType, Algorithm>(char[N])
	template<size_t N, typename CharType = char, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr StringHash<CharType, Algorithm> Hash(const CharType(&cstr)[N]) noexcept
	{
		return StringHash<CharType, Algorithm>{ cstr };
	}

	/// StringHash<CharType, Algorithm>(cstring)
	template<typename CharType = char, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr StringHash<CharType, Algorithm> Hash(const typename Epic::StringHash<CharType, Algorithm>::CStringWrapper str) noexcept
	{
		return StringHash<CharType, Algorithm>{ str };
	}

	/// StringHash<CharType, Algorithm>(std::string)
	template<typename CharType = char, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default,
			 typename Traits = std::char_traits<CharType>, typename Allocator = std::allocator<CharType>>
	constexpr StringHash<CharType, Algorithm> Hash(const std::basic_string<CharType, Traits, Allocator>& str) noexcept
	{
		return StringHash<CharType, Algorithm>{ str };
	}
}

//////////////////////////////////////////////////////////////////////////////

/// UDLs
namespace Epic
{
	namespace UDL
	{
		// String Hash UDLs
		inline Epic::StringHash<char> operator "" _hash(const char* str, size_t /*len*/)
		{
			return Epic::StringHash<char>(str);
		}

		inline Epic::StringHash<wchar_t> operator "" _hash(const wchar_t* str, size_t /*len*/)
		{
			return Epic::StringHash<wchar_t>(str);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

/// std::hash<Epic::StringHash<C, A>>
namespace std
{
	template<typename C, Epic::StringHashAlgorithms A>
	struct hash<Epic::StringHash<C, A>>
	{
		constexpr hash() noexcept = default;

		constexpr size_t operator() (const Epic::StringHash<C, A>& x) const noexcept
		{
			return static_cast<size_t>(x.Value());
		}
	};
}
