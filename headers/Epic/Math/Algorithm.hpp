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
#include <cassert>
#include <cmath>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace
	{
		// Calculate the distance between two Vectors
		template<class T, class U, size_t Size>
		inline auto Distance(const Vector<T, Size>& vecA, const Vector<U, Size>& vecB)
		{
			return (vecA - vecB).Magnitude();
		}

		// Calculate the distance between two VectorSwizzlers
		template<class VectorT1, class VectorT2, class TArray1, class TArray2, size_t... Indices1, size_t... Indices2,
			typename = std::enable_if_t<(sizeof...(Indices1) == sizeof...(Indices2))>>
		inline auto Distance(const VectorSwizzler<VectorT1, TArray1, Indices1...>& vecA,
							 const VectorSwizzler<VectorT2, TArray2, Indices2...>& vecB)
		{
			return Distance(vecA.ToVector(), vecB.ToVector());
		}

		// Calculate the distance between a Vector and a VectorSwizzler
		template<class T, size_t Size, class VectorType, class TArray, size_t... Indices,
			typename = std::enable_if_t<(Size == sizeof...(Indices))>>
		inline auto Distance(const Vector<T, Size>& vecA, const VectorSwizzler<VectorType, TArray, Indices...>& vecB)
		{
			return Distance(vecA, vecB.ToVector());
		}
		
		// Calculate the distance between a Vector and a VectorSwizzler
		template<class T, size_t Size, class VectorType, class TArray, size_t... Indices,
			typename = std::enable_if_t<(Size == sizeof...(Indices))>>
		inline auto Distance(const VectorSwizzler<VectorType, TArray, Indices...>& vecA, const Vector<T, Size>& vecB)
		{
			return Distance(vecA.ToVector(), vecB);
		}

		// Calculate the average value of Vector 'vec'
		template<class T, size_t Size>
		inline auto Mean(const Vector<T, Size>& vec)
		{
			T result = T(0);

			Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Size>>
				::Apply([&](size_t n) { result += vec[n]; });

			return result / T(Size);
		}

		// Calculate the average value of VectorSwizzler 'vec'
		template<class VectorType, class TArray, size_t... Indices>
		inline auto Mean(const VectorSwizzler<VectorType, TArray, Indices...>& vec)
		{
			return Mean(vec.ToVector());
		}

		// Calculate the weighted average value of Vector 'vec'
		template<class T, size_t SizeV, class U, size_t SizeW>
		inline auto WeightedMean(const Vector<T, SizeV>& vec, const U(&weights)[SizeW])
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
		template<class T, size_t Size, class U>
		inline auto WeightedMean(const Vector<T, Size>& vec, std::initializer_list<U> weights)
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
		template<class VectorType, class TArray, size_t... Indices, class U, size_t Size>
		inline auto WeightedMean(const VectorSwizzler<VectorType, TArray, Indices...>& vec, const U(&weights)[Size])
		{
			return WeightedMean(vec.ToVector(), weights);
		}

		// Calculate the weighted average value of VectorSwizzler 'vec'
		template<class VectorType, class TArray, size_t... Indices, class U>
		inline auto WeightedMean(const VectorSwizzler<VectorType, TArray, Indices...>& vec, std::initializer_list<U> weights)
		{
			return WeightedMean(vec.ToVector(), weights);
		}
	}
}