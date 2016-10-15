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
	using Type = Epic::BasicStringHash<C, A>;
	using CharType = C;
	using AlgorithmType = Epic::StringHashAlgorithm<C, A>;
	using HashType = typename AlgorithmType::HashType;

	static constexpr Epic::StringHashAlgorithms Algorithm = A;

private:
	HashType _Hash;

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
	constexpr BasicStringHash() noexcept
		: _Hash{ Epic::StringHashAlgorithm<C, A>::Hash(nullptr) } { }

	template<size_t N>
	constexpr BasicStringHash(const CharType(&cstr)[N]) noexcept
		: _Hash{ Epic::StringHashAlgorithm<C, A>::FoldHash(cstr) } { }

	inline BasicStringHash(CStringWrapper cstr) noexcept
		: _Hash{ Epic::StringHashAlgorithm<C, A>::Hash(cstr.Str) } { }

	template<typename Traits, typename Allocator>
	inline BasicStringHash(const std::basic_string<C, Traits, Allocator>& str) noexcept
		: _Hash{ Epic::StringHashAlgorithm<C, A>::Hash(str.c_str()) } { }

public:
	constexpr operator HashType (void) const noexcept
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
		constexpr bool operator Op (const Epic::BasicStringHash<C, A>& rhs) const noexcept	\
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
	/// Hash<N, Algorithm>(char[N])
	template<size_t N, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr BasicStringHash<char, Algorithm> Hash(const char(&cstr)[N]) noexcept
	{
		return BasicStringHash<char, Algorithm>{ cstr };
	}

	/// Hash<N, Algorithm>(wchar_t[N])
	template<size_t N, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr BasicStringHash<wchar_t, Algorithm> Hash(const wchar_t(&cstr)[N]) noexcept
	{
		return BasicStringHash<wchar_t, Algorithm>{ cstr };
	}

	/// Hash<Algorithm>(cstring)
	template<Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr BasicStringHash<char, Algorithm> Hash(const typename Epic::BasicStringHash<char, Algorithm>::CStringWrapper str) noexcept
	{
		return BasicStringHash<char, Algorithm>{ str };
	}

	/// Hash<Algorithm>(wide cstring)
	template<Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr BasicStringHash<wchar_t, Algorithm> Hash(const typename Epic::BasicStringHash<wchar_t, Algorithm>::CStringWrapper str) noexcept
	{
		return BasicStringHash<wchar_t, Algorithm>{ str };
	}

	/// Hash<Algorithm>(std::string)
	template<class Traits, class Allocator, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr BasicStringHash<char, Algorithm> Hash(const std::basic_string<char, Traits, Allocator>& str) noexcept
	{
		return BasicStringHash<char, Algorithm>{ str };
	}

	/// Hash<Algorithm>(std::wstring)
	template<class Traits, class Allocator, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr BasicStringHash<wchar_t, Algorithm> Hash(const std::basic_string<wchar_t, Traits, Allocator>& str) noexcept
	{
		return BasicStringHash<wchar_t, Algorithm>{ str };
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
