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

#include <Epic/Math/Vector.hpp>
#include <Epic/TMP/Sequence.hpp>
#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

// Constants
namespace Epic
{
	namespace
	{
		template<typename T = float>
		constexpr T Pi = T(3.1415926535897932385);

		template<typename T = float>
		constexpr T PiSq = Pi<T> * Pi<T>;

		template<typename T = float>
		constexpr T TwoPi = Pi<T> +Pi<T>;

		template<typename T = float>
		constexpr T HalfPi = Pi<T> / T(2);

		template<typename T = float>
		constexpr T InvPi = T(1) / Pi<T>;

		template<typename T = float>
		constexpr T InvTwoPi = T(1) / TwoPi<T>;

		template<typename T = float>
		constexpr T Epsilon = std::numeric_limits<T>::epsilon();

		template<typename T = float>
		constexpr T MaxReal = std::numeric_limits<T>::max();

		template<typename T = float>
		constexpr T MinReal = std::numeric_limits<T>::min();
	}
}

//////////////////////////////////////////////////////////////////////////////

// Angle Algorithms
namespace Epic
{
	namespace
	{
		// Convert a degree value to radians
		template<typename T>
		constexpr T DegToRad(const T value) noexcept
		{
			return Pi<T> * value / T(180);
		}

		// Convert a radian value to degrees
		template<typename T>
		constexpr T RadToDeg(const T value) noexcept
		{
			return T(180) * value / Pi<T>;
		}
	}
}

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

		// Calculate the distance between two VectorSwizzlers
		template<class VectorT1, class VectorT2, class TArray1, class TArray2, size_t... Indices1, size_t... Indices2,
				 typename = std::enable_if_t<(sizeof...(Indices1) == sizeof...(Indices2))>>
		inline auto Distance(const VectorSwizzler<VectorT1, TArray1, Indices1...>& vecA,
							 const VectorSwizzler<VectorT2, TArray2, Indices2...>& vecB) noexcept
		{
			return Distance(vecA.ToVector(), vecB.ToVector());
		}

		// Calculate the distance between a Vector and a VectorSwizzler
		template<class T, size_t Size, class VectorType, class TArray, size_t... Indices,
				 typename = std::enable_if_t<(Size == sizeof...(Indices))>>
		inline auto Distance(const Vector<T, Size>& vecA, 
							 const VectorSwizzler<VectorType, TArray, Indices...>& vecB) noexcept
		{
			return Distance(vecA, vecB.ToVector());
		}
		
		// Calculate the distance between a Vector and a VectorSwizzler
		template<class T, size_t Size, class VectorType, class TArray, size_t... Indices,
				 typename = std::enable_if_t<(Size == sizeof...(Indices))>>
		inline auto Distance(const VectorSwizzler<VectorType, TArray, Indices...>& vecA, 
							 const Vector<T, Size>& vecB) noexcept
		{
			return Distance(vecA.ToVector(), vecB);
		}

		// Calculate the average value of Vector 'vec'
		template<class T, size_t Size, 
				 typename = std::enable_if_t<(Size > 0)>>
		inline auto Mean(const Vector<T, Size>& vec) noexcept
		{
			T result = T(0);

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Size>>
				::Apply([&](size_t n) { result += vec[n]; });

			return result / T(Size);
		}

		// Calculate the average value of VectorSwizzler 'vec'
		template<class VectorType, class TArray, size_t... Indices, 
				 typename = std::enable_if_t<(sizeof...(Indices) > 0)>>
		inline auto Mean(const VectorSwizzler<VectorType, TArray, Indices...>& vec) noexcept
		{
			return Mean(vec.ToVector());
		}

		// Calculate the weighted average value of Vector 'vec'
		template<class T, size_t SizeV, class U, size_t SizeW, 
				 typename = std::enable_if_t<(SizeV > 0)>>
		inline auto WeightedMean(const Vector<T, SizeV>& vec, const U(&weights)[SizeW]) noexcept
		{
			T result = T(0);
			T tweights = T(0);
			
			Epic::TMP::ForEach2<
				Epic::TMP::MakeSequence<size_t, SizeV>,
				Epic::TMP::MakeSequence<size_t, SizeW>>
			::Apply([&](size_t nv, size_t nw)
					{
						result += vec[nv] * weights[nw];
						tweights += weights[nw];
					});

			assert(tweights != T(0));

			return result / tweights;
		}

		// Calculate the weighted average value of Vector 'vec'
		template<class T, size_t Size, class U, 
				 typename = std::enable_if_t<(Size > 0)>>
		inline auto WeightedMean(const Vector<T, Size>& vec, std::initializer_list<U> weights) noexcept
		{
			T result = T(0);
			T tweights = T(0);
			auto it = std::begin(weights);

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Size>>
				::Apply([&](size_t n)
						{
							if (it != std::end(weights))
							{
								result += vec[n] * (*it);
								tweights += (*it);
								++it;
							}
						});

			assert(tweights != T(0));

			return result / tweights;
		}

		// Calculate the weighted average value of VectorSwizzler 'vec'
		template<class VectorType, class TArray, size_t... Indices, class U, size_t Size, 
				 typename = std::enable_if_t<(sizeof...(Indices) > 0)>>
		inline auto WeightedMean(const VectorSwizzler<VectorType, TArray, Indices...>& vec, 
								 const U(&weights)[Size]) noexcept
		{
			return WeightedMean(vec.ToVector(), weights);
		}

		// Calculate the weighted average value of VectorSwizzler 'vec'
		template<class VectorType, class TArray, size_t... Indices, class U, 
				 typename = std::enable_if_t<(sizeof...(Indices) > 0)>>
		inline auto WeightedMean(const VectorSwizzler<VectorType, TArray, Indices...>& vec, 
								 std::initializer_list<U> weights) noexcept
		{
			return WeightedMean(vec.ToVector(), weights);
		}

		// Calculate the negative of color 'vec'
		template<class T, size_t Size, 
				 typename = std::enable_if_t<(Size > 0)>>
		inline auto Negative(const Vector<T, Size>& vec, const T max = T(1)) noexcept
		{
			return Vector<T, Size>(max) - vec;
		}

		// Calculate the negative of color 'vec'
		template<class VectorType, class TArray, size_t... Indices, 
				 typename = std::enable_if<(sizeof...(Indices) > 0)>>
		inline auto Negative(const VectorSwizzler<VectorType, TArray, Indices...>& vec, 
							 const typename VectorType::ValueType max = typename VectorType::ValueType(1)) noexcept
		{
			return Negative(vec.ToVector(), max);
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
