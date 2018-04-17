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

// Literal
namespace Epic::TMP
{
	template<typename T, T N>
	using Literal = std::integral_constant<T, N>;
}

//////////////////////////////////////////////////////////////////////////////

// GenIndexList
namespace Epic::TMP
{
	// GenIndexList<I> - Generates a List<> containing Literal<size_t> types from 0..I.
	template<size_t I>
	struct GenIndexList
	{
		using Type = typename Concat<
			typename GenIndexList<I - 1>::type, 
			List<TMP::Literal<size_t, I>>
		>::type;
	};

	template<> 
	struct GenIndexList<0>
	{
		using type = List<TMP::Literal<size_t, 0>>;
	};
	
	// IndexListFor<Ts...> - Invokes GenIndexList on the sizeof... Ts
	template<typename... Ts>
	using IndexListFor = typename GenIndexList<sizeof...(Ts) - 1>::type;
}

//////////////////////////////////////////////////////////////////////////////

// DebugSwitch
namespace Epic::TMP
{
	template<class DebugType, class ReleaseType>
	struct DebugSwitch;

#ifdef NDEBUG
	template<class, class R>
	struct DebugSwitch
	{
		using type = R;
	};
#else
	template<class D, class> 
	struct DebugSwitch
	{
		using type = D;
	};
#endif
}

//////////////////////////////////////////////////////////////////////////////

// StaticMax
namespace Epic::TMP
{
	template<size_t... Vs>
	struct StaticMax;

	template<size_t V1>
	struct StaticMax<V1>
	{
		static constexpr size_t value = V1;
	};
	
	template<size_t V1, size_t... Vs>
	struct StaticMax<V1, Vs...>
	{
		static constexpr size_t next = StaticMax<Vs...>::value;
		static constexpr size_t value = (V1 > next) ? V1 : next;
	};
}
