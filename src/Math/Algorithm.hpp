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
		#pragma region Epic::Distance

		// Calculate the distance between two Vectors
		template<class T, class U, std::size_t Size>
		inline auto Distance(const Vector<T, Size>& vecA, const Vector<U, Size>& vecB) noexcept
		{
			return (vecA - vecB).Magnitude();
		}

		// Calculate the distance between two Swizzlers
		template<class T1, class T2, std::size_t TS1, std::size_t TS2, 
				 std::size_t... Indices1, std::size_t... Indices2,
				 typename = std::enable_if_t<(sizeof...(Indices1) == sizeof...(Indices2))>>
		inline auto Distance(const Swizzler<T1, TS1, Indices1...>& vecA,
							 const Swizzler<T2, TS2, Indices2...>& vecB) noexcept
		{
			return Distance(vecA.ToVector(), vecB.ToVector());
		}

		// Calculate the distance between a Vector and a Swizzler
		template<class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
				 typename = std::enable_if_t<(Size == sizeof...(Indices))>>
		inline auto Distance(const Vector<T1, Size>& vecA, 
							 const Swizzler<T2, TS2, Indices...>& vecB) noexcept
		{
			return Distance(vecA, vecB.ToVector());
		}
		
		// Calculate the distance between a Vector and a Swizzler
		template<class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
				 typename = std::enable_if_t<(Size == sizeof...(Indices))>>
		inline auto Distance(const Swizzler<T2, TS2, Indices...>& vecA, 
							 const Vector<T1, Size>& vecB) noexcept
		{
			return Distance(vecA.ToVector(), vecB);
		}

		#pragma endregion

		#pragma region Epic::Min

		// Calculate the min values of two Vectors
		template<class T, std::size_t SizeA, std::size_t SizeB>
		inline auto Min(const Vector<T, SizeA>& vecA, const Vector<T, SizeB>& vecB) noexcept
		{
			static constexpr std::size_t N = (SizeA > SizeB) ? SizeA : SizeB;
			
			Vector<T, N> result;

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<std::size_t, N>>::Apply([&](std::size_t i) 
			{
				if (i >= SizeA)
					result[i] = vecB[i];
				else if (i >= SizeB)
					result[i] = vecA[i];
				else
					result[i] = std::min(vecA[i], vecB[i]);
			});

			return result;
		}

		// Calculate the min values of two Vectors using 'comp' to compare elements
		template<class Compare, class T, std::size_t SizeA, std::size_t SizeB>
		inline auto Min(const Vector<T, SizeA>& vecA, const Vector<T, SizeB>& vecB, Compare comp) noexcept
		{
			static constexpr std::size_t N = (SizeA > SizeB) ? SizeA : SizeB;
			
			Vector<T, N> result;

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<std::size_t, N>>::Apply([&](std::size_t i) 
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

		// Calculate the min values of two Swizzlers
		template<class T1, class T2, std::size_t TS1, std::size_t TS2, 
				 std::size_t... Indices1, std::size_t... Indices2,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Min(const Swizzler<T1, TS1, Indices1...>& vecA,
						const Swizzler<T2, TS2, Indices2...>& vecB) noexcept
		{
			return Min(vecA.ToVector(), vecB.ToVector());
		}

		// Calculate the min values of two Swizzlers using 'comp' to compare elements
		template<class Compare, class T1, class T2, std::size_t TS1, std::size_t TS2, 
				 std::size_t... Indices1, std::size_t... Indices2,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Min(const Swizzler<T1, TS1, Indices1...>& vecA,
						const Swizzler<T2, TS2, Indices2...>& vecB,
						Compare comp) noexcept
		{
			return Min(vecA.ToVector(), vecB.ToVector(), comp);
		}

		// Calculate the min values of a Vector and a Swizzler
		template<class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Min(const Vector<T1, Size>& vecA, 
						const Swizzler<T2, TS2, Indices...>& vecB) noexcept
		{
			return Min(vecA, vecB.ToVector());
		}
		
		// Calculate the min values of a Vector and a Swizzler using 'comp' to compare elements
		template<class Compare, class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Min(const Vector<T1, Size>& vecA, 
						const Swizzler<T2, TS2, Indices...>& vecB,
						Compare comp) noexcept
		{
			return Min(vecA, vecB.ToVector(), comp);
		}
		
		// Calculate the min values of a Swizzler and a Vector
		template<class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Min(const Swizzler<T2, TS2, Indices...>& vecA, 
						const Vector<T1, Size>& vecB) noexcept
		{
			return Min(vecA.ToVector(), vecB);
		}

		// Calculate the min values of a Swizzler and a Vector using 'comp' to compare elements
		template<class Compare, class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Min(const Swizzler<T2, TS2, Indices...>& vecA, 
						const Vector<T1, Size>& vecB,
						Compare comp) noexcept
		{
			return Min(vecA.ToVector(), vecB, comp);
		}

		#pragma endregion

		#pragma region Epic::Max

		// Calculate the min values of two Vectors
		template<class T, std::size_t SizeA, std::size_t SizeB>
		inline auto Max(const Vector<T, SizeA>& vecA, const Vector<T, SizeB>& vecB) noexcept
		{
			static constexpr std::size_t N = (SizeA > SizeB) ? SizeA : SizeB;
			
			Vector<T, N> result;

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<std::size_t, N>>::Apply([&](std::size_t i) 
			{
				if (i >= SizeA)
					result[i] = vecB[i];
				else if (i >= SizeB)
					result[i] = vecA[i];
				else
					result[i] = std::max(vecA[i], vecB[i]);
			});

			return result;
		}

		// Calculate the min values of two Vectors using 'comp' to compare elements
		template<class Compare, class T, std::size_t SizeA, std::size_t SizeB>
		inline auto Max(const Vector<T, SizeA>& vecA, const Vector<T, SizeB>& vecB, Compare comp) noexcept
		{
			static constexpr std::size_t N = (SizeA > SizeB) ? SizeA : SizeB;
			
			Vector<T, N> result;

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<std::size_t, N>>::Apply([&](std::size_t i) 
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

		// Calculate the min values of two Swizzlers
		template<class T1, class T2, std::size_t TS1, std::size_t TS2, 
				 std::size_t... Indices1, std::size_t... Indices2,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Max(const Swizzler<T1, TS1, Indices1...>& vecA,
						const Swizzler<T2, TS2, Indices2...>& vecB) noexcept
		{
			return Max(vecA.ToVector(), vecB.ToVector());
		}

		// Calculate the min values of two Swizzlers using 'comp' to compare elements
		template<class Compare, class T1, class T2, std::size_t TS1, std::size_t TS2, 
				 std::size_t... Indices1, std::size_t... Indices2,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Max(const Swizzler<T1, TS1, Indices1...>& vecA,
						const Swizzler<T2, TS2, Indices2...>& vecB,
						Compare comp) noexcept
		{
			return Max(vecA.ToVector(), vecB.ToVector(), comp);
		}

		// Calculate the min values of a Vector and a Swizzler
		template<class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Max(const Vector<T1, Size>& vecA, 
						const Swizzler<T2, TS2, Indices...>& vecB) noexcept
		{
			return Max(vecA, vecB.ToVector());
		}
		
		// Calculate the min values of a Vector and a Swizzler using 'comp' to compare elements
		template<class Compare, class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Max(const Vector<T1, Size>& vecA, 
						const Swizzler<T2, TS2, Indices...>& vecB,
						Compare comp) noexcept
		{
			return Max(vecA, vecB.ToVector(), comp);
		}
		
		// Calculate the min values of a Swizzler and a Vector
		template<class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Max(const Swizzler<T2, TS2, Indices...>& vecA, 
						const Vector<T1, Size>& vecB) noexcept
		{
			return Max(vecA.ToVector(), vecB);
		}

		// Calculate the min values of a Swizzler and a Vector using 'comp' to compare elements
		template<class Compare, class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
				 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
		inline auto Max(const Swizzler<T2, TS2, Indices...>& vecA, 
						const Vector<T1, Size>& vecB,
						Compare comp) noexcept
		{
			return Max(vecA.ToVector(), vecB, comp);
		}

		#pragma endregion

		#pragma region Epic::Clamp

		// Clamp the values of a Vector
		template<class T, std::size_t Size>
		inline auto Clamp(const Vector<T, Size>& vec, const T& low, const T& high) noexcept
		{
			return Clamp(vec, low, high, std::less<>());
		}

		// Clamp the values of a Vector using 'comp' to compare elements
		template<class Compare, class T, std::size_t Size>
		inline auto Clamp(const Vector<T, Size>& vec, const T& low, const T& high, Compare comp) noexcept
		{
			assert(!comp(high, low));
			
			Vector<T, Size> result;

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<std::size_t, Size>>::Apply([&](std::size_t i) 
			{
				result[i] = comp(vec[i], low) ? low : comp(high, vec[i]) ? high : vec[i];
			});

			return result;
		}

		// Clamp the values of a Swizzler
		template<class T, std::size_t TS, std::size_t... Is>
		inline auto Clamp(const Swizzler<T, TS, Is...>& vec, const T& low, const T& high) noexcept
		{
			return Clamp(vec.ToVector(), low, high, std::less<>());
		}

		// Clamp the values of a Swizzler using 'comp' to compare elements
		template<class Compare, class T, std::size_t TS, std::size_t... Is>
		inline auto Clamp(const Swizzler<T, TS, Is...>& vec, const T& low, const T& high, Compare comp) noexcept
		{
			return Clamp(vec.ToVector(), low, high, comp);
		}

		#pragma endregion

		#pragma region Epic::Lerp

		template<class T, class F, typename EnabledForFloatingPoint = std::enable_if_t<std::is_floating_point<F>::value>>
		inline auto Lerp(const T& from, const T& to, const F t) noexcept
		{
			return from + ((to - from) * t);
		}

		#pragma endregion

		// Calculate the angle between unit vectors 'vecA' and 'vecB'
		template<class T, class U, std::size_t Size>
		inline Radian<T> AngleOf(const Vector<T, Size>& vecA, const Vector<U, Size>& vecB) noexcept
		{
			return{ std::acos(vecA.Dot(vecB)) };
		}

		#pragma region Epic::Mean / Epic::WeightedMean

		// Calculate the average value of Vector 'vec'
		template<class T, std::size_t Size, typename = std::enable_if_t<(Size > 0)>>
		inline auto Mean(const Vector<T, Size>& vec) noexcept
		{
			T result = T(0);

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<std::size_t, Size>>
				::Apply([&](std::size_t n) { result += vec[n]; });

			return result / T(Size);
		}

		// Calculate the average value of Swizzler 'vec'
		template<class T, std::size_t TS, std::size_t... Indices,
				 typename = std::enable_if_t<(sizeof...(Indices) > 0)>>
		inline auto Mean(const Swizzler<T, TS, Indices...>& vec) noexcept
		{
			return Mean(vec.ToVector());
		}

		// Calculate the weighted average value of Vector 'vec'
		template<class T, std::size_t Size, class U, 
				 typename = std::enable_if_t<(Size > 0)>>
		inline auto WeightedMean(const Vector<T, Size>& vec, const U(&weights)[Size]) noexcept
		{
			T result = T(0);
			T tweights = T(0);
			
			Epic::TMP::ForEach<Epic::TMP::MakeSequence<std::size_t, Size>>
				::Apply([&](std::size_t n)
				{
					result += vec[n] * weights[n];
					tweights += weights[n];
				});

			assert(tweights != T(0));

			return result / tweights;
		}

		// Calculate the weighted average value of Swizzler 'vec'
		template<class T, std::size_t TS, std::size_t... Indices, class U,
				 typename = std::enable_if_t<(sizeof...(Indices) > 0)>>
		inline auto WeightedMean(const Swizzler<T, TS, Indices...>& vec, 
								 const U(&weights)[sizeof...(Indices)]) noexcept
		{
			return WeightedMean(vec.ToVector(), weights);
		}

		#pragma endregion

		#pragma region Epic::Negative

		// Calculate the negative of color 'vec'
		template<class T, std::size_t Size, 
				 typename = std::enable_if_t<(Size > 0)>>
		inline auto Negative(const Vector<T, Size>& vec, const T ceil = T(1)) noexcept
		{
			return Vector<T, Size>(ceil) - vec;
		}

		// Calculate the negative of color 'vec'
		template<class T, std::size_t TS, std::size_t... Indices, 
				 typename = std::enable_if<(sizeof...(Indices) > 0)>>
		inline auto Negative(const Swizzler<T, TS, Indices...>& vec, const T ceil = T(1)) noexcept
		{
			return Negative(vec.ToVector(), ceil);
		}

		#pragma endregion
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
		inline uint64_t SqrtPow2(const uint64_t value) noexcept
		{
			static const uint64_t lookup[] = 
			{ 
				0xAAAAAAAAAAAAAAAA, 
				0xCCCCCCCCCCCCCCCC, 
				0xF0F0F0F0F0F0F0F0,
				0xFF00FF00FF00FF00, 
				0xFFFF0000FFFF0000, 
				0xFFFFFFFF00000000 
			};

			uint64_t result = (value & lookup[0]) != 0 ? uint64_t(1) : uint64_t(0);

			result |= ((value & lookup[5]) != 0 ? uint64_t(1) : uint64_t(0)) << 5;
			result |= ((value & lookup[4]) != 0 ? uint64_t(1) : uint64_t(0)) << 4;
			result |= ((value & lookup[3]) != 0 ? uint64_t(1) : uint64_t(0)) << 3;
			result |= ((value & lookup[2]) != 0 ? uint64_t(1) : uint64_t(0)) << 2;
			result |= ((value & lookup[1]) != 0 ? uint64_t(1) : uint64_t(0)) << 1;

			return (uint64_t(1) << (result >> 1));
		}

		inline uint32_t SqrtPow2(const uint32_t value) noexcept
		{
			static const uint32_t lookup[] = 
			{ 
				0xAAAAAAAA, 
				0xCCCCCCCC, 
				0xF0F0F0F0, 
				0xFF00FF00, 
				0xFFFF0000 
			};

			uint32_t result = (value & lookup[0]) != 0 ? uint32_t(1) : uint32_t(0);

			result |= ((value & lookup[4]) != 0 ? uint32_t(1) : uint32_t(0)) << 4;
			result |= ((value & lookup[3]) != 0 ? uint32_t(1) : uint32_t(0)) << 3;
			result |= ((value & lookup[2]) != 0 ? uint32_t(1) : uint32_t(0)) << 2;
			result |= ((value & lookup[1]) != 0 ? uint32_t(1) : uint32_t(0)) << 1;

			return (uint32_t(1) << (result >> 1));
		}

		inline uint16_t SqrtPow2(const uint16_t value) noexcept
		{
			static const uint16_t lookup[] = 
			{ 
				0xAAAA, 
				0xCCCC, 
				0xF0F0, 
				0xFF00 
			};

			uint16_t result = (value & lookup[0]) != 0 ? uint16_t(1) : uint16_t(0);

			result |= ((value & lookup[3]) != 0 ? uint16_t(1) : uint16_t(0)) << 3;
			result |= ((value & lookup[2]) != 0 ? uint16_t(1) : uint16_t(0)) << 2;
			result |= ((value & lookup[1]) != 0 ? uint16_t(1) : uint16_t(0)) << 1;

			return (uint16_t(1) << (result >> 1));
		}

		inline uint8_t SqrtPow2(const uint8_t value) noexcept
		{
			static const uint8_t lookup[] = 
			{ 
				0xAA, 
				0xCC, 
				0xF0 
			};
			
			uint8_t result = (value & lookup[0]) != 0 ? uint8_t(1) : uint8_t(0);

			result |= ((value & lookup[2]) != 0 ? uint8_t(1) : uint8_t(0)) << 2;
			result |= ((value & lookup[1]) != 0 ? uint8_t(1) : uint8_t(0)) << 1;

			return (uint8_t(1) << (result >> 1));
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
		inline T NextPower2(const T value) noexcept
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
	#pragma region std::min

	// Calculate the min values of two Epic::Vectors
	template<class T, std::size_t SizeA, std::size_t SizeB>
	inline auto min(const Epic::Vector<T, SizeA>& vecA, const Epic::Vector<T, SizeB>& vecB) noexcept
	{
		return Epic::Min(vecA, vecB);
	}

	// Calculate the min values of two Epic::Vectors using 'comp' to compare elements
	template<class Compare, class T, std::size_t SizeA, std::size_t SizeB>
	inline auto min(const Epic::Vector<T, SizeA>& vecA, const Epic::Vector<T, SizeB>& vecB, Compare comp) noexcept
	{
		return Epic::Min(vecA, vecB, comp);
	}

	// Calculate the min values of two Epic::Swizzlers
	template<class T1, class T2, std::size_t TS1, std::size_t TS2, 
			 std::size_t... Indices1, std::size_t... Indices2,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto min(const Epic::Swizzler<T1, TS1, Indices1...>& vecA,
					const Epic::Swizzler<T2, TS2, Indices2...>& vecB) noexcept
	{
		return Epic::Min(vecA, vecB);
	}

	// Calculate the min values of two Epic::Swizzlers using 'comp' to compare elements
	template<class Compare, class T1, class T2, std::size_t TS1, std::size_t TS2, 
			 std::size_t... Indices1, std::size_t... Indices2,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto min(const Epic::Swizzler<T1, TS1, Indices1...>& vecA,
					const Epic::Swizzler<T2, TS2, Indices2...>& vecB,
					Compare comp) noexcept
	{
		return Epic::Min(vecA, vecB, comp);
	}

	// Calculate the min values of an Epic::Vector and an Epic::Swizzler
	template<class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto min(const Epic::Vector<T1, Size>& vecA, 
					const Epic::Swizzler<T2, TS2, Indices...>& vecB) noexcept
	{
		return Epic::Min(vecA, vecB);
	}
		
	// Calculate the min values of an Epic::Vector and an Epic::Swizzler using 'comp' to compare elements
	template<class Compare, class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto min(const Epic::Vector<T1, Size>& vecA, 
					const Epic::Swizzler<T2, TS2, Indices...>& vecB,
					Compare comp) noexcept
	{
		return Epic::Min(vecA, vecB, comp);
	}
		
	// Calculate the min values of an Epic::Swizzler and an Epic::Vector
	template<class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto min(const Epic::Swizzler<T2, TS2, Indices...>& vecA, 
					const Epic::Vector<T1, Size>& vecB) noexcept
	{
		return Epic::Min(vecA, vecB);
	}

	// Calculate the min values of an Epic::Swizzler and an Epic::Vector using 'comp' to compare elements
	template<class Compare, class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto min(const Epic::Swizzler<T2, TS2, Indices...>& vecA, 
					const Epic::Vector<T1, Size>& vecB,
					Compare comp) noexcept
	{
		return Epic::Min(vecA, vecB, comp);
	}

	#pragma endregion

	#pragma region std::max

	// Calculate the max values of two Epic::Vectors
	template<class T, std::size_t SizeA, std::size_t SizeB>
	inline auto max(const Epic::Vector<T, SizeA>& vecA, const Epic::Vector<T, SizeB>& vecB) noexcept
	{
		return Epic::Max(vecA, vecB);
	}

	// Calculate the max values of two Epic::Vectors using 'comp' to compare elements
	template<class Compare, class T, std::size_t SizeA, std::size_t SizeB>
	inline auto max(const Epic::Vector<T, SizeA>& vecA, const Epic::Vector<T, SizeB>& vecB, Compare comp) noexcept
	{
		return Epic::Max(vecA, vecB, comp);
	}

	// Calculate the max values of two Epic::Swizzlers
	template<class T1, class T2, std::size_t TS1, std::size_t TS2, 
			 std::size_t... Indices1, std::size_t... Indices2,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto max(const Epic::Swizzler<T1, TS1, Indices1...>& vecA,
					const Epic::Swizzler<T2, TS2, Indices2...>& vecB) noexcept
	{
		return Epic::Max(vecA, vecB);
	}

	// Calculate the max values of two Epic::Swizzlers using 'comp' to compare elements
	template<class Compare, class T1, class T2, std::size_t TS1, std::size_t TS2, 
			 std::size_t... Indices1, std::size_t... Indices2,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto max(const Epic::Swizzler<T1, TS1, Indices1...>& vecA,
					const Epic::Swizzler<T2, TS2, Indices2...>& vecB,
					Compare comp) noexcept
	{
		return Epic::Max(vecA, vecB, comp);
	}

	// Calculate the max values of an Epic::Vector and an Epic::Swizzler
	template<class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto max(const Epic::Vector<T1, Size>& vecA, 
					const Epic::Swizzler<T2, TS2, Indices...>& vecB) noexcept
	{
		return Epic::Max(vecA, vecB);
	}
		
	// Calculate the max values of an Epic::Vector and an Epic::Swizzler using 'comp' to compare elements
	template<class Compare, class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto max(const Epic::Vector<T1, Size>& vecA, 
					const Epic::Swizzler<T2, TS2, Indices...>& vecB,
					Compare comp) noexcept
	{
		return Epic::Max(vecA, vecB, comp);
	}
		
	// Calculate the max values of an Epic::Swizzler and an Epic::Vector
	template<class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto max(const Epic::Swizzler<T2, TS2, Indices...>& vecA, 
					const Epic::Vector<T1, Size>& vecB) noexcept
	{
		return Epic::Max(vecA, vecB);
	}

	// Calculate the max values of an Epic::Swizzler and an Epic::Vector using 'comp' to compare elements
	template<class Compare, class T1, std::size_t Size, class T2, std::size_t TS2, std::size_t... Indices,
			 typename = std::enable_if_t<std::is_same<T1, T2>::value>>
	inline auto max(const Epic::Swizzler<T2, TS2, Indices...>& vecA, 
					const Epic::Vector<T1, Size>& vecB,
					Compare comp) noexcept
	{
		return Epic::Max(vecA, vecB, comp);
	}

	#pragma endregion

	#pragma region std::clamp

	// Clamp the values of a Vector
	template<class T, std::size_t Size>
	inline auto clamp(const Epic::Vector<T, Size>& vec, const T& low, const T& high) noexcept
	{
		return Epic::Clamp(vec, low, high);
	}

	// Clamp the values of a Vector using 'comp' to compare elements
	template<class Compare, class T, std::size_t Size>
	inline auto clamp(const Epic::Vector<T, Size>& vec, const T& low, const T& high, Compare comp) noexcept
	{
		return Epic::Clamp(vec, low, high, comp);
	}

	// Clamp the values of a Swizzler
	template<class T, std::size_t TS, std::size_t... Indices>
	inline auto clamp(const Epic::Swizzler<T, TS, Indices...>& vec, const T& low, const T& high) noexcept
	{
		return Epic::Clamp(vec, low, high);
	}

	// Clamp the values of a Swizzler using 'comp' to compare elements
	template<class Compare, class T, std::size_t TS, std::size_t... Indices>
	inline auto clamp(const Epic::Swizzler<T, TS, Indices...>& vec, const T& low, const T& high, Compare comp) noexcept
	{
		return Epic::Clamp(vec, low, high, comp);
	}

	#pragma endregion
}
