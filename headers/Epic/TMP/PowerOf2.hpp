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

#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace TMP
	{
		template<size_t> struct IsPowerOf2;
		template<size_t> struct FloorPowerOf2;
		template<size_t> struct CeilPowerOf2;
	}
}

//////////////////////////////////////////////////////////////////////////////

///  IsPowerOf2<N> : value = whether or not N is a power of 2
template<size_t N>
struct Epic::TMP::IsPowerOf2
	: std::conditional_t<!(N & (N - 1)), std::true_type, std::false_type> { };

/// IsPowerOf2<0> : value = false
template<>
struct Epic::TMP::IsPowerOf2<0> : std::false_type { };

//////////////////////////////////////////////////////////////////////////////

/// FloorPowerOf2<N> : value = the largest power of 2 that is <= N
template<size_t N>
struct Epic::TMP::FloorPowerOf2
	: std::integral_constant<size_t, FloorPowerOf2<N / 2>::value * 2> { };

/// FloorPowerOf2<1> : value = 1
template<>
struct Epic::TMP::FloorPowerOf2<1> : std::integral_constant<size_t, 1> { };

/// FloorPowerOf2<0> : value = 0
template<>
struct Epic::TMP::FloorPowerOf2<0> : std::integral_constant<size_t, 0> { };

//////////////////////////////////////////////////////////////////////////////

/// CeilPowerOf2<N> : value = the smallest power of 2 that is >= N
template<size_t N>
struct Epic::TMP::CeilPowerOf2
	: std::integral_constant<size_t, IsPowerOf2<N>::value ? N : FloorPowerOf2<N>::value * 2> { };

/// CeilPowerOf2<0> : value = 1
template<>
struct Epic::TMP::CeilPowerOf2<0> : std::integral_constant<size_t, 1> { };
