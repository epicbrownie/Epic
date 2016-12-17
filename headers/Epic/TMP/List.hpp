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

// List<> - List Metatype
namespace Epic::TMP
{
	template<typename...> struct List;
}

//////////////////////////////////////////////////////////////////////////////

// Concat<> - List Concatenation
namespace Epic::TMP
{
	template<typename... Lists> 
	struct Concat;

	template<>
	struct Concat<>
	{
		using Type = List<>;
	};

	template<typename... Ts>
	struct Concat<List<Ts...>>
	{
		using Type = List<Ts...>;
	};

	template<typename... Ts, typename... Us>
	struct Concat<List<Ts...>, List<Us...>>
	{
		using Type = List<Ts..., Us...>;
	};

	template<typename... Ts, typename... Us, typename... Rest>
	struct Concat<List<Ts...>, List<Us...>, Rest...>
	{
		using Type = typename Concat<List<Ts..., Us...>, Rest...>::Type;
	};
}

//////////////////////////////////////////////////////////////////////////////

// Filter<> - List Filtering
namespace Epic::TMP
{
	namespace detail
	{
		template<template<typename> class Pred, typename T>
		using FilterHelper = std::conditional_t<Pred<T>::value, List<T>, List<>>;
	}

	template<template<typename> class Pred, typename... Ts>
	using Filter = typename Concat<detail::FilterHelper<Pred, Ts>...>::Type;
}
