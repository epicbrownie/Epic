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

/// StringHashAlgorithm<C, PaulLarson>
template<typename C>
class Epic::StringHashAlgorithm<C, Epic::StringHashAlgorithms::PaulLarson>
{
public:
	using CharType = C;
	using HashType = uint64_t;

	static constexpr Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::PaulLarson;

private:
	static constexpr HashType DefaultSeed{ 2166136261 };

public:
	StringHashAlgorithm() = delete;

	static constexpr HashType Hash(std::nullptr_t, const HashType seed = DefaultSeed) noexcept
	{
		return seed;
	}

	static inline HashType Hash(const CharType* __restrict str = nullptr, const HashType seed = DefaultSeed) noexcept
	{
		HashType hash = seed;

		while (*str != 0)
			hash = hash * 101 + *str++;

		return hash;
	}

private:
	template<size_t N, size_t I>
	struct _FoldHash
	{
		static constexpr HashType apply(const CharType(&str)[N], const HashType seed) noexcept
		{
			return (_FoldHash<N, (I - 1)>::apply(str, seed) * 101) + str[I - 1];
		}
	};

	template<size_t N>
	struct _FoldHash<N, 0>
	{
		static constexpr HashType apply(const CharType(&str)[N], const HashType seed) noexcept
		{
			return seed;
		}
	};

public:
	template<size_t N>
	static constexpr HashType FoldHash(const CharType(&str)[N], const HashType seed = DefaultSeed) noexcept
	{
		return _FoldHash<N, (N - 1)>::apply(str, seed);
	}
};

//////////////////////////////////////////////////////////////////////////////

/// StringHashAlgorithm<C, FNV-1a>
template<typename C>
class Epic::StringHashAlgorithm<C, Epic::StringHashAlgorithms::FNV1a>
{
public:
	using CharType = C;
	using HashType = uint64_t;

	static constexpr Epic::StringHashAlgorithms Algorithm = Epic::StringHashAlgorithms::FNV1a;

private:
	static constexpr HashType DefaultSeed{ 2166136261 };
	static constexpr HashType DefaultPrime{ 16777619 };

public:
	StringHashAlgorithm() = delete;

	static constexpr HashType Hash(std::nullptr_t, const HashType seed = DefaultSeed, const HashType = DefaultPrime) noexcept
	{
		return seed;
	}

	static inline HashType Hash(const CharType* __restrict str, const HashType seed = DefaultSeed, const HashType prime = DefaultPrime) noexcept
	{
		HashType hash = seed;
		
		while (*str != 0)
			hash = (hash ^ *str++) * prime;

		return hash;
	}

private:
	template<size_t N, size_t I>
	struct _FoldHash
	{
		static constexpr HashType apply(const CharType(&str)[N], const HashType seed, const HashType prime) noexcept
		{
			return (_FoldHash<N, (I - 1)>::apply(str, seed, prime) ^ str[I - 1]) * prime;
		}
	};

	template<size_t N>
	struct _FoldHash<N, 0>
	{
		static constexpr HashType apply(const CharType(&str)[N], const HashType seed, const HashType /* prime */) noexcept
		{
			return seed;
		}
	};

public:
	template<size_t N>
	static constexpr HashType FoldHash(const CharType(&str)[N], const HashType seed = DefaultSeed, const HashType prime = DefaultPrime) noexcept
	{
		return _FoldHash<N, (N - 1)>::apply(str, seed, prime);
	}
};
