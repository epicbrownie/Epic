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

#include <limits>

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
		constexpr T TwoPi = Pi<T> + Pi<T>;

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
