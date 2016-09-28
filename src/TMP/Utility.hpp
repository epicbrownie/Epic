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
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
	template<size_t I> 
	struct GenIndexSequence;

	template<class DebugType, class ReleaseType> 
	struct DebugSwitch;

	template<size_t... Vs>
	struct StaticMax;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
	template<typename T, T N>
	using Literal = std::integral_constant<T, N>;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
	/// GenIndexSequence<I> - Generates a List<> containing Literal<size_t> types from 0..I.
	template<size_t I>
	struct GenIndexSequence
	{
		using Type = typename Concat<typename GenIndexSequence<I - 1>::Type, List<TMP::Literal<size_t, I>>>::Type;
	};

	template<> 
	struct GenIndexSequence<0>
	{
		using Type = List<TMP::Literal<size_t, 0>>;
	};
	

	/// IndexSequenceFor<Ts...> - Invokes GenIndexSequence on the size of Ts
	template<typename... Ts>
	using IndexSequenceFor = typename GenIndexSequence<sizeof...(Ts) - 1>::Type;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
#ifdef NDEBUG
	template<class, class R>
	struct DebugSwitch
	{
		using Type = R;
	};
#else
	template<class D, class> 
	struct DebugSwitch
	{
		using Type = D;
	};
#endif
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
	template<size_t V1>
	struct StaticMax<V1>
	{
		static constexpr size_t value = V1;
	};
	
	template<size_t V1, size_t... Vs>
	struct StaticMax<V1, Vs...>
	{
		static constexpr size_t value = (V1 > StaticMax<Vs...>::value) ? V1 : StaticMax<Vs...>::value;
	};
}
