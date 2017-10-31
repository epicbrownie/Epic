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
#include <cstring>
#include <functional>
#include <string>
#include <string_view>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<typename CharType = char, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	class BasicStringHash;
}

//////////////////////////////////////////////////////////////////////////////

/// BasicStringHash<C, A>
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
	HashType m_Hash;

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
		: m_Hash{ AlgorithmType::Hash(nullptr, 0) } { }

	template<size_t N>
	constexpr BasicStringHash(const CharType(&cstr)[N]) noexcept
		: m_Hash{ AlgorithmType::Hash<N>(cstr) } { }

	constexpr BasicStringHash(CStringWrapper cstr) noexcept
		: m_Hash{ AlgorithmType::Hash(cstr.Str, std::strlen(cstr.Str)) } { }

	template<typename Traits>
	constexpr BasicStringHash(const std::basic_string_view<CharType, Traits> str) noexcept
		: m_Hash{ AlgorithmType::Hash(str.data(), str.length()) } { }

	template<typename Traits, typename Alloc>
	constexpr BasicStringHash(const std::basic_string<CharType, Traits, Alloc>& str) noexcept
		: m_Hash{ AlgorithmType::Hash(str.data(), str.length()) } { }

	template<class C2>
	constexpr BasicStringHash(const BasicStringHash<C2, A>& other) noexcept
		: m_Hash{ other.Value() } { }

public:
	constexpr operator HashType () const noexcept
	{
		return m_Hash;
	}

	constexpr Type& operator = (std::nullptr_t) noexcept
	{
		m_Hash = AlgorithmType::Hash(nullptr, 0);
		return *this;
	}

	template<size_t N>
	constexpr Type& operator = (const CharType(&cstr)[N]) noexcept
	{
		m_Hash = AlgorithmType::Hash(cstr);
		return *this;
	}

	constexpr Type& operator = (const CStringWrapper cstr) noexcept
	{
		m_Hash = AlgorithmType::Hash(cstr.Str, std::strlen(cstr.Str));
		return *this;
	}

	template<typename Traits>
	constexpr Type& operator = (const std::basic_string_view<CharType, Traits> str) noexcept
	{
		m_Hash = AlgorithmType::Hash(str.data(), str.length());
		return *this;
	}

	template<typename Traits, typename Alloc>
	constexpr Type& operator = (const std::basic_string<CharType, Traits, Alloc>& str) noexcept
	{
		m_Hash = AlgorithmType::Hash(str.data(), str.length());
		return *this;
	}

	template<class C2>
	constexpr Type& operator = (const BasicStringHash<C2, A>& other) noexcept
	{
		m_Hash = other.Value();
		return *this;
	}

public:
	constexpr HashType Value() const noexcept
	{
		return m_Hash;
	}

public:
	#define CREATE_COMPARISON_OPERATOR(Op)	\
		template<class C2>																		\
		constexpr bool operator Op (const Epic::BasicStringHash<C2, A>& rhs) const noexcept		\
		{																						\
			return m_Hash Op rhs.Value();														\
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

	/// Hash<Algorithm>(std::string_view)
	template<Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default, class Traits>
	constexpr BasicStringHash<char, Algorithm> Hash(const std::basic_string_view<char, Traits> str) noexcept
	{
		return BasicStringHash<char, Algorithm>{ str };
	}

	/// Hash<Algorithm>(std::wstring_view)
	template<Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default, class Traits>
	constexpr BasicStringHash<wchar_t, Algorithm> Hash(const std::basic_string_view<wchar_t, Traits> str) noexcept
	{
		return BasicStringHash<wchar_t, Algorithm>{ str };
	}

	/// Hash<Algorithm>(std::string)
	template<class Traits, class Alloc, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr BasicStringHash<char, Algorithm> Hash(const std::basic_string<char, Traits, Alloc>& str) noexcept
	{
		return BasicStringHash<char, Algorithm>{ str };
	}

	/// Hash<Algorithm>(std::wstring)
	template<class Traits, class Alloc, Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::Default>
	constexpr BasicStringHash<wchar_t, Algorithm> Hash(const std::basic_string<wchar_t, Traits, Alloc>& str) noexcept
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
