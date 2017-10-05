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
#include <type_traits>

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

	static constexpr HashType Hash(const HashType seed = DefaultSeed) noexcept
	{
		return seed;
	}

	static constexpr HashType Hash(const CharType* __restrict str, const size_t length, const HashType seed = DefaultSeed) noexcept
	{
		HashType hash = seed;

		for(size_t i = 0; i < length; ++i)
			hash = hash * 101 + str[i];

		return hash;
	}

	template<size_t N, typename = std::enable_if_t<(N > 0)>>
	static constexpr HashType Hash(const CharType(&str)[N], const HashType seed = DefaultSeed) noexcept
	{
		HashType hash = seed;

		for (size_t i = 0; i < (N - 1); ++i)
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

	static constexpr HashType Hash(const HashType seed = DefaultSeed)
	{
		return seed;
	}

	static constexpr HashType Hash(const CharType* __restrict str, const size_t length, const HashType seed = DefaultSeed, const HashType prime = DefaultPrime) noexcept
	{
		HashType hash = seed;
		
		for (size_t i = 0; i < length; ++i)
			hash = (hash ^ str[i]) * prime;

		return hash;
	}

	template<size_t N, typename = std::enable_if_t<(N > 0)>>
	static constexpr HashType Hash(const char(&str)[N], const HashType seed = DefaultSeed, const HashType prime = DefaultPrime) noexcept
	{
		HashType hash = seed;

		for (size_t i = 0; i < N - 1; ++i)
			hash = (hash ^ str[i]) * prime;

		return hash;
	}
};
