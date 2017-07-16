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

#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

#pragma warning(disable: 4307)	// C4307 warns against integral constant overflow

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	enum class StringHashAlgorithms
	{
		PaulLarson,
		FNV1a,

		Default = FNV1a
	};
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<typename CharType = char, StringHashAlgorithms Algorithm = StringHashAlgorithms::Default>
	struct StringHashAlgorithm;
}

//////////////////////////////////////////////////////////////////////////////

/// StringHashAlgorithm<C, PaulLarson>
template<typename C>
struct Epic::StringHashAlgorithm<C, Epic::StringHashAlgorithms::PaulLarson>
{
	using CharType = C;
	using HashType = std::uint32_t;

	static constexpr Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::PaulLarson;
	static constexpr HashType DefaultSeed{ 2166136261 };

	static constexpr HashType Hash(std::nullptr_t, const HashType seed = DefaultSeed) noexcept
	{
		return seed;
	}

	static constexpr HashType Hash(const CharType* __restrict str = nullptr, const HashType seed = DefaultSeed) noexcept
	{
		HashType hash = seed;

		while (*str != 0)
			hash = hash * 101 + *str++;

		return hash;
	}

	template<size_t N>
	static constexpr HashType Hash(const CharType(&str)[N], const HashType seed = DefaultSeed) noexcept
	{
		HashType hash = seed;

		for (size_t i = 0; i < N; ++i)
			hash = (hash * 101) + str[i];

		return hash;
	}
};

//////////////////////////////////////////////////////////////////////////////

/// StringHashAlgorithm<C, FNV-1a>
template<typename C>
struct Epic::StringHashAlgorithm<C, Epic::StringHashAlgorithms::FNV1a>
{
	using CharType = C;
	using HashType = std::uint32_t;

	static constexpr Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::FNV1a;
	static constexpr HashType DefaultSeed{ 2166136261 };
	static constexpr HashType DefaultPrime{ 16777619 };

	static constexpr HashType Hash(std::nullptr_t, const HashType seed = DefaultSeed, const HashType = DefaultPrime) noexcept
	{
		return seed;
	}

	static constexpr HashType Hash(const CharType* __restrict str, const HashType seed = DefaultSeed, const HashType prime = DefaultPrime) noexcept
	{
		HashType hash = seed;
		
		while (*str != 0)
			hash = (hash ^ *str++) * prime;

		return hash;
	}

	template<size_t N>
	static constexpr HashType Hash(const char(&str)[N], const HashType seed = DefaultSeed, const HashType prime = DefaultPrime) noexcept
	{
		HashType hash = seed;

		for (size_t i = 0; i < N; ++i)
			hash = (hash ^ str[i]) * prime;

		return hash;
	}
};
