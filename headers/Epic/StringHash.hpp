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
	class BasicStringHash;
}

//////////////////////////////////////////////////////////////////////////////

/// StringHash<C, A>
template<typename C, Epic::StringHashAlgorithms A>
class Epic::BasicStringHash
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
	constexpr BasicStringHash(const CharType(&cstr)[N]) noexcept
		: _Hash{ Epic::StringHashAlgorithm<C, A>::FoldHash(cstr) } { }

	explicit inline BasicStringHash(CStringWrapper cstr) noexcept
		: _Hash{ Epic::StringHashAlgorithm<C, A>::Hash(cstr.Str) } { }

	template<typename Traits, typename Allocator>
	explicit inline BasicStringHash(const std::basic_string<C, Traits, Allocator>& str) noexcept
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
		inline constexpr bool operator Op (const Epic::BasicStringHash<C, A>& rhs) const noexcept	\
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

/// Aliases
namespace Epic
{
	using StringHash = Epic::BasicStringHash<char, Epic::StringHashAlgorithms::Default>;
	using WStringHash = Epic::BasicStringHash<wchar_t, Epic::StringHashAlgorithms::Default>;
}

//////////////////////////////////////////////////////////////////////////////

/// Shortcuts
namespace Epic
{
	/// BasicStringHash<N, CharType, Algorithm>(char[N])
	template<size_t N, typename CharType = char, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr BasicStringHash<CharType, Algorithm> Hash(const CharType(&cstr)[N]) noexcept
	{
		return BasicStringHash<CharType, Algorithm>{ cstr };
	}

	/// BasicStringHash<CharType, Algorithm>(cstring)
	template<typename CharType = char, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr BasicStringHash<CharType, Algorithm> Hash(const typename Epic::BasicStringHash<CharType, Algorithm>::CStringWrapper str) noexcept
	{
		return BasicStringHash<CharType, Algorithm>{ str };
	}

	/// BasicStringHash<CharType, Algorithm>(std::string)
	template<typename CharType = char, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default,
			 typename Traits = std::char_traits<CharType>, typename Allocator = std::allocator<CharType>>
	constexpr BasicStringHash<CharType, Algorithm> Hash(const std::basic_string<CharType, Traits, Allocator>& str) noexcept
	{
		return BasicStringHash<CharType, Algorithm>{ str };
	}
}

//////////////////////////////////////////////////////////////////////////////

/// std::hash<Epic::BasicStringHash<C, A>>
namespace std
{
	template<typename C, Epic::StringHashAlgorithms A>
	struct hash<Epic::BasicStringHash<C, A>>
	{
		constexpr hash() noexcept = default;

		constexpr size_t operator() (const Epic::BasicStringHash<C, A>& x) const noexcept
		{
			return static_cast<size_t>(x.Value());
		}
	};
}
