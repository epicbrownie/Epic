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
		template<class SearchType, class... VariadicTypes>
		struct VariadicContains;
	}
}

//////////////////////////////////////////////////////////////////////////////

/// VariadicContains<T> : End of variadic list
template<class T>
struct Epic::TMP::VariadicContains<T> : std::false_type { };

/// VariadicContains<T, V, ...> : Variadic list head is not same as T
template<class T, class V, class... Vs>
struct Epic::TMP::VariadicContains<T, V, Vs...>
	: Epic::TMP::VariadicContains<T, Vs...> { };

/// VariadicContains<T, T, ...> : Variadic list head is same as T
template<class T, class... Vs>
struct Epic::TMP::VariadicContains<T, T, Vs...>
	: std::true_type { };
