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
#include <string>

//////////////////////////////////////////////////////////////////////////////

#pragma warning(push)
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
	class HashedString;

	template<typename CharType = char, StringHashAlgorithms Algorithm = StringHashAlgorithms::Default>
	class StringHashAlgorithm;
}

//////////////////////////////////////////////////////////////////////////////

/// StringHashAlgorithm<CharType, A>
template<typename CharType, Epic::StringHashAlgorithms A>
class Epic::StringHashAlgorithm { };

//////////////////////////////////////////////////////////////////////////////

/// StringHashAlgorithm<CharType, PaulLarson>
template<typename CharType>
class Epic::StringHashAlgorithm<CharType, Epic::StringHashAlgorithms::PaulLarson>
{
private:
	static constexpr size_t DefaultSeed{ 2166136261 };

public:
	StringHashAlgorithm() = delete;

	static inline size_t Hash(const CharType* __restrict str, const size_t seed = DefaultSeed) noexcept
	{
		size_t hash = seed;

		while (*str != 0)
			hash = hash * 101 + *str++;

		return hash;
	}

private:
	template<size_t N, size_t I>
	struct _FoldHash
	{
		static constexpr size_t apply(const CharType(&str)[N], const size_t seed) noexcept
		{
			return (_FoldHash<N, (I - 1)>::apply(str, seed) * 101) + str[I - 1];
		}
	};

	template<size_t N>
	struct _FoldHash<N, 0>
	{
		static constexpr size_t apply(const CharType(&str)[N], const size_t seed) noexcept
		{
			return seed;
		}
	};

public:
	template<size_t N>
	static constexpr size_t FoldHash(const CharType(&str)[N], const size_t seed = DefaultSeed) noexcept
	{
		return _FoldHash<N, (N - 1)>::apply(str, seed);
	}
};

//////////////////////////////////////////////////////////////////////////////

/// StringHashAlgorithm<CharType, FNV-1a>
template<typename CharType>
class Epic::StringHashAlgorithm<CharType, Epic::StringHashAlgorithms::FNV1a>
{
private:
	static constexpr size_t DefaultSeed{ 2166136261 };
	static constexpr size_t DefaultPrime{ 16777619 };

public:
	StringHashAlgorithm() = delete;

	static inline size_t Hash(const CharType* __restrict str, const size_t seed = DefaultSeed, const size_t prime = DefaultPrime) noexcept
	{
		size_t hash = seed;

		while (*str != 0)
		{
			hash ^= *str++;
			hash *= prime;
		}

		return hash;
	}

private:
	template<size_t N, size_t I>
	struct _FoldHash
	{
		static constexpr size_t apply(const CharType(&str)[N], const size_t seed, const size_t prime) noexcept
		{
			return (_FoldHash<N, (I - 1)>::apply(str, seed, prime) ^ str[I - 1]) * prime;
		}
	};

	template<size_t N>
	struct _FoldHash<N, 0>
	{
		static constexpr size_t apply(const CharType(&str)[N], const size_t seed, const size_t /* prime */) noexcept
		{
			return seed;
		}
	};

public:
	template<size_t N>
	static constexpr size_t FoldHash(const CharType(&str)[N], const size_t seed = DefaultSeed, const size_t prime = DefaultPrime) noexcept
	{
		return _FoldHash<N, (N - 1)>::apply(str, seed, prime);
	}
};

//////////////////////////////////////////////////////////////////////////////

#pragma warning(pop)
