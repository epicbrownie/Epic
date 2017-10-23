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

#include <Epic/Math/Angle.hpp>
#include <Epic/Math/Constants.hpp>
#include <Epic/Math/Vector.hpp>
#include <Epic/TMP/Sequence.hpp>
#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

// Vector Algorithms
namespace Epic
{
	namespace
	{
		// Calculate the distance between two Vectors
		template<class T, class U, size_t Size>
		inline auto Distance(const Vector<T, Size>& vecA, const Vector<U, Size>& vecB) noexcept
		{
			return (vecA - vecB).Magnitude();
		}

		// Calculate the min values of two Vectors using 'comp' to compare elements
		template<class T, size_t SizeA, size_t SizeB, class Compare = std::less<T>>
		inline auto Min(const Vector<T, SizeA>& vecA, const Vector<T, SizeB>& vecB, Compare comp = Compare()) noexcept
		{
			static constexpr size_t N = (SizeA > SizeB) ? SizeA : SizeB;
			
			Vector<T, N> result;

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, N>>::Apply([&](size_t i) 
			{
				if (i >= SizeA)
					result[i] = vecB[i];
				else if (i >= SizeB)
					result[i] = vecA[i];
				else
					result[i] = std::min(vecA[i], vecB[i], comp);
			});

			return result;
		}

		// Calculate the max values of two Vectors using 'comp' to compare elements
		template<class T, size_t SizeA, size_t SizeB, class Compare = std::less<T>>
		inline auto Max(const Vector<T, SizeA>& vecA, const Vector<T, SizeB>& vecB, Compare comp = Compare()) noexcept
		{
			static constexpr size_t N = (SizeA > SizeB) ? SizeA : SizeB;
			
			Vector<T, N> result;

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, N>>::Apply([&](size_t i) 
			{
				if (i >= SizeA)
					result[i] = vecB[i];
				else if (i >= SizeB)
					result[i] = vecA[i];
				else
					result[i] = std::max(vecA[i], vecB[i], comp);
			});

			return result;
		}

		// Clamp the values of a Vector using 'comp' to compare elements
		template<class T, size_t Size, class Compare = std::less<T>>
		inline auto Clamp(const Vector<T, Size>& vec, 
						  const Vector<T, Size>& low, 
						  const Vector<T, Size>& high, 
						  Compare comp = Compare()) noexcept
		{
			Epic::TMP::ForEachN<Size>::Apply([&](size_t i)
			{
				assert(!comp(high[i], low[i]));
			});
			
			Vector<T, Size> result;

			Epic::TMP::ForEachN<Size>::Apply([&](size_t i) 
			{
				result[i] = comp(vec[i], low[i]) ? low[i] : comp(high[i], vec[i]) ? high[i] : vec[i];
			});

			return result;
		}

		// Clamp the values of a Vector using 'comp' to compare elements
		template<class U, class T, size_t Size, class Compare = std::less<T>>
		inline auto Clamp(const Vector<T, Size>& vec, U&& low, U&& high, Compare comp = Compare()) noexcept
		{
			return Clamp<T, Size, Compare>
			(
				vec, 
				Vector<T, Size>(std::forward<U>(low)), 
				Vector<T, Size>(std::forward<U>(high))
			);
		}

		template<class T, class F, typename EnabledForFloatingPoint = std::enable_if_t<std::is_floating_point<F>::value>>
		inline auto Lerp(const T& from, const T& to, const F t) noexcept
		{
			return from + ((to - from) * t);
		}

		// Calculate the angle between unit vectors 'vecA' and 'vecB'
		template<class T, class U, size_t Size>
		inline Radian<T> AngleOf(const Vector<T, Size>& vecA, const Vector<U, Size>& vecB) noexcept
		{
			return{ std::acos(vecA.Dot(vecB)) };
		}

		// Calculate the average value of Vector 'vec'
		template<class T, size_t Size, typename = std::enable_if_t<(Size > 0)>>
		inline auto Mean(const Vector<T, Size>& vec) noexcept
		{
			T result = T(0);

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Size>>
				::Apply([&](size_t n) { result += vec[n]; });

			return result / T(Size);
		}

		// Calculate the weighted average value of Vector 'vec'
		template<class T, size_t Size, class U, 
				 typename = std::enable_if_t<(Size > 0)>>
		inline auto WeightedMean(const Vector<T, Size>& vec, const U(&weights)[Size]) noexcept
		{
			T result = T(0);
			T tweights = T(0);
			
			Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Size>>
				::Apply([&](size_t n)
				{
					result += vec[n] * weights[n];
					tweights += weights[n];
				});

			assert(tweights != T(0));

			return result / tweights;
		}

		// Calculate the negative of color 'vec'
		template<class T, size_t Size, 
				 typename = std::enable_if_t<(Size > 0)>>
		inline auto Negative(const Vector<T, Size>& vec, const T ceil = T(1)) noexcept
		{
			return Vector<T, Size>(ceil) - vec;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// Misc Algorithms
namespace Epic
{
	namespace
	{
		// Round floating type F to nearest integral T
		template<typename T, typename F, 
				 typename = std::enable_if_t<std::is_floating_point<F>::value && std::is_integral<T>::value>>
		constexpr T Round(const F value) noexcept
		{
			return T(value + F(0.5));
		}

		// Calculate the square root of 'value' ('value' must be a power of 2)
		constexpr uint64_t SqrtPow2(const uint64_t value) noexcept
		{
			constexpr uint64_t lookup[] = 
			{ 
				0xAAAAAAAAAAAAAAAA, 
				0xCCCCCCCCCCCCCCCC, 
				0xF0F0F0F0F0F0F0F0,
				0xFF00FF00FF00FF00, 
				0xFFFF0000FFFF0000, 
				0xFFFFFFFF00000000 
			};

			const uint64_t result =
				  ((uint64_t)((value & lookup[0]) != 0) << 0)
				| ((uint64_t)((value & lookup[1]) != 0) << 1)
				| ((uint64_t)((value & lookup[2]) != 0) << 2)
				| ((uint64_t)((value & lookup[3]) != 0) << 3)
				| ((uint64_t)((value & lookup[4]) != 0) << 4)
				| ((uint64_t)((value & lookup[5]) != 0) << 5);

			return uint64_t(1) << (result >> 1);
		}

		constexpr uint32_t SqrtPow2(const uint32_t value) noexcept
		{
			constexpr uint32_t lookup[] = 
			{ 
				0xAAAAAAAA, 
				0xCCCCCCCC, 
				0xF0F0F0F0, 
				0xFF00FF00, 
				0xFFFF0000 
			};

			const uint32_t result = 
				((uint32_t)((value & lookup[0]) != 0) << 0)
			  | ((uint32_t)((value & lookup[1]) != 0) << 1)
			  | ((uint32_t)((value & lookup[2]) != 0) << 2)
			  | ((uint32_t)((value & lookup[3]) != 0) << 3)
			  | ((uint32_t)((value & lookup[4]) != 0) << 4);

			return uint32_t(1) << (result >> 1);
		}

		constexpr uint16_t SqrtPow2(const uint16_t value) noexcept
		{
			constexpr uint16_t lookup[] = 
			{ 
				0xAAAA, 
				0xCCCC, 
				0xF0F0, 
				0xFF00 
			};

			const uint16_t result = 
				((uint16_t)((value & lookup[0]) != 0) << 0)
			  | ((uint16_t)((value & lookup[1]) != 0) << 1)
			  | ((uint16_t)((value & lookup[2]) != 0) << 2)
			  | ((uint16_t)((value & lookup[3]) != 0) << 3);

			return uint16_t(1) << (result >> 1);
		}

		constexpr uint8_t SqrtPow2(const uint8_t value) noexcept
		{
			constexpr uint8_t lookup[] = 
			{ 
				0xAA, 
				0xCC, 
				0xF0 
			};
			
			uint8_t result = 
				((uint8_t)((value & lookup[0]) != 0) << 0)
			  | ((uint8_t)((value & lookup[1]) != 0) << 1)
			  | ((uint8_t)((value & lookup[2]) != 0) << 2);

			return uint8_t(1) << (result >> 1);
		}

		// Calculate whether or not 'value' is a power of 2
		template<class T, typename = std::enable_if_t<std::is_unsigned<T>::value>>
		constexpr bool IsPower2(const T value) noexcept
		{
			return !!!(value & (value - T(1)));
		}

		// Calculate 'x' % 'y' when 'y' is a power of 2
		template<class T, typename = std::enable_if_t<std::is_unsigned<T>::value>>
		constexpr T ModPower2(const T x, const T y) noexcept
		{
			return x & (y - T(1));
		}

		// Calculate the next greater 'value' that is a power of 2
		template<class T, typename = std::enable_if_t<std::is_unsigned<T>::value>>
		constexpr T NextPower2(const T value) noexcept
		{
			T result = T(1);
			while (result < value) result <<= 1;
			return result;
		}

		// Calculate the first prime number that is smaller than 'value'
		template<class T, typename = std::enable_if_t<std::is_integral<T>::value>>
		T SmallerPrime(const T value) noexcept
		{
			T candidate = T(2);
			T result = T(0);

			while (candidate <= value)
			{
				T trialDivisor = T(2);
				bool isPrime = true;

				while (trialDivisor * trialDivisor <= candidate)
				{
					if (candidate % trialDivisor == 0)
					{
						isPrime = false;
						break;
					}

					++trialDivisor;
				}

				if (isPrime)
					result = candidate;

				++candidate;
			}

			return result;
		}

		// Calculate the first prime number that is larger than 'value'
		template<class T, typename = std::enable_if_t<std::is_integral<T>::value>>
		T LargerPrime(const T value) noexcept
		{
			T candidate = T(2);
			T result = T(0);

			while (result <= value)
			{
				if (candidate == Epic::MaxReal<T>)
					return T(0);

				T trialDivisor = T(2);
				bool isPrime = true;

				while (trialDivisor * trialDivisor <= candidate)
				{
					if (candidate % trialDivisor == 0)
					{
						isPrime = false;
						break;
					}

					++trialDivisor;
				}

				if (isPrime)
					result = candidate;

				++candidate;
			}

			return result;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// std Algorithms
namespace std
{
	// Calculate the min values of two Epic::Vectors using 'comp' to compare elements
	template<class T, size_t SizeA, size_t SizeB, class Compare = std::less<T>>
	inline auto min(const Epic::Vector<T, SizeA>& vecA, const Epic::Vector<T, SizeB>& vecB, Compare comp = Compare()) noexcept
	{
		return Epic::Min(vecA, vecB, comp);
	}

	// Calculate the max values of two Epic::Vectors using 'comp' to compare elements
	template<class T, size_t SizeA, size_t SizeB, class Compare = std::less<T>>
	inline auto max(const Epic::Vector<T, SizeA>& vecA, const Epic::Vector<T, SizeB>& vecB, Compare comp = Compare()) noexcept
	{
		return Epic::Max(vecA, vecB, comp);
	}

	// Clamp the values of a Vector using 'comp' to compare elements
	template<class T, size_t Size, class Compare = std::less<T>>
	inline auto clamp(const Epic::Vector<T, Size>& vec, 
					  const Epic::Vector<T, Size>& low = Epic::Zero, 
					  const Epic::Vector<T, Size>& high = Epic::One, 
					  Compare comp = Compare()) noexcept
	{
		return Epic::Clamp(vec, low, high, comp);
	}

	// Clamp the values of a Vector using 'comp' to compare elements
	template<class U, class T, size_t Size, class Compare = std::less<T>>
	inline auto clamp(const Epic::Vector<T, Size>& vec, 
					  U&& low = T(0), U&& high = T(1), 
					  Compare comp = Compare()) noexcept
	{
		return Epic::Clamp<T, Size, Compare>(vec, std::forward<U>(low), std::forward<U>(high), comp);
	}
}
