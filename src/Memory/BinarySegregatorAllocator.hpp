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

#include <Epic/Memory/SegregatorAllocator.hpp>
#include <Epic/Memory/SegBucket.hpp>
#include <Epic/TMP/List.hpp>
#include <Epic/TMP/Utility.hpp>
#include <tuple>
#include <utility>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	// Bucket List Filter
	template<template<typename> class Pred, typename T, typename U>
	using SegBucketListFilterHelper = typename std::conditional<Pred<U>::value, TMP::List<T>, TMP::List<>>::type;

	template<template<typename> class Pred, typename Is, typename... Ts>
	struct SegBucketListFilter;

	template<template<typename> class Pred, typename... Is, typename... Ts>
	struct SegBucketListFilter<Pred, TMP::List<Is...>, Ts...>
	{
		using Type = typename TMP::Concat<SegBucketListFilterHelper<Pred, Ts, Is>...>::Type;
	};


	// Bucket List Filter Predicates
	template<size_t N>
	struct SegBucketIndexLess
	{
		template<typename I>
		struct Predicate
		{
			static constexpr bool value{ I::value < N };
		};
	};

	template<size_t N>
	struct SegBucketIndexGreater
	{
		template<typename I>
		struct Predicate
		{
			static constexpr bool value{ I::value > N };
		};
	};


	// SegregatorAllocator Builder
	template<typename... Args>
	struct BinarySegregatorAllocatorBuilder;

	template<typename... Buckets>
	struct BinarySegregatorAllocatorBuilder<TMP::List<Buckets...>>
	{
		static constexpr size_t N{ ((sizeof...(Buckets)) - 1) / 2 };

		using Center = std::tuple_element_t<N, std::tuple<Buckets...>>;
		
		using Left = typename SegBucketListFilter<
			SegBucketIndexLess<N>::Predicate, 
			TMP::IndexListFor<Buckets...>, 
			Buckets...>::Type;
		
		using Right = typename SegBucketListFilter<
			SegBucketIndexGreater<N>::Predicate, 
			TMP::IndexListFor<Buckets...>, 
			Buckets...>::Type;

		using Type = typename BinarySegregatorAllocatorBuilder<Center, Left, Right, Buckets...>::Type;
	};

	template<size_t T, class A, typename... Ls, typename... Rs, typename... Buckets>
	struct BinarySegregatorAllocatorBuilder<SegBucket<T, A>, TMP::List<Ls...>, TMP::List<Rs...>, Buckets...>
	{
		using Type = Epic::SegregatorAllocator<T,
			typename BinarySegregatorAllocatorBuilder<typename TMP::Concat<TMP::List<Ls...>, TMP::List<A>>::Type>::Type,
			typename BinarySegregatorAllocatorBuilder<TMP::List<Rs...>>::Type>;
	};

	template<class A>
	struct BinarySegregatorAllocatorBuilder<TMP::List<A>>
	{
		using Type = A;
	};

	template<size_t T, class A, class B>
	struct BinarySegregatorAllocatorBuilder<TMP::List<SegBucket<T, A>, B>>
	{
		using Type = Epic::SegregatorAllocator<T, A, B>;
	};


	// MakeSegBucketList
	template<typename... Args>
	struct MakeSegBucketList;

	template<size_t T, class A, typename... Args>
	struct MakeSegBucketList<SegBucket<T, A>, Args...>
	{
		using Type = typename TMP::Concat<
			TMP::List<SegBucket<T, A>>, 
			typename MakeSegBucketList<Args...>::Type>::Type;
	};

	template<size_t T, class A, typename... Args>
	struct MakeSegBucketList<TMP::Literal<size_t, T>, A, Args...>
	{
		using Type = typename TMP::Concat<
			TMP::List<SegBucket<T, A>>, 
			typename MakeSegBucketList<Args...>::Type>::Type;
	};

	template<class A>
	struct MakeSegBucketList<A>
	{
		using Type = TMP::List<A>;
	};


	// BinarySegregatorAllocatorBuilder Invoker
	template<typename... Args>
	using BinarySegregatorAllocatorBuilderInvoker =
		typename BinarySegregatorAllocatorBuilder<typename MakeSegBucketList<Args...>::Type>::Type;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<typename... Args>
	using BinarySegregatorAllocator = typename Epic::detail::BinarySegregatorAllocatorBuilderInvoker<Args...>::Type;
}
