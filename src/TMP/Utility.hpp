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

#include <Epic/TMP/List.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
	template<size_t I> struct GenIndexSequence;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
	/// GenIndexSequence<I> - Generates a List<> containing std::integral_constant<size_t> types from 0..I.
	template<size_t I>
	struct GenIndexSequence
	{
		using type = typename Concat<typename GenIndexSequence<I - 1>::type, List<std::integral_constant<size_t, I>>>::type;
	};

	template<> 
	struct GenIndexSequence<0>
	{
		using type = List<std::integral_constant<size_t, 0>>;
	};
	

	/// IndexSequenceFor<Ts...> - Invokes GenIndexSequence on the size of Ts
	template<typename... Ts>
	using IndexSequenceFor = typename GenIndexSequence<sizeof...(Ts) - 1>::type;
}
