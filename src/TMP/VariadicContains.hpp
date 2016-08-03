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

namespace Epic::TMP::detail
{
	template<class, class...>
	struct VariadicContainsImpl;

	template<class T>
	struct VariadicContainsImpl<T> : std::false_type { };

	template<class T, class V, class... Vs>
	struct VariadicContainsImpl<T, V, Vs...> : VariadicContainsImpl<T, Vs...> { };

	template<class T, class... Vs>
	struct VariadicContainsImpl<T, T, Vs...> : std::true_type { };
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
	template<class SearchType, class... VariadicTypes>
	using VariadicContains = detail::VariadicContainsImpl<SearchType, VariadicTypes...>;
}
