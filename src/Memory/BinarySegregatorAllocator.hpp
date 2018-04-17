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
	using SegBucketListFilterHelper = std::conditional_t<Pred<U>::value, TMP::List<T>, TMP::List<>>;

	template<template<typename> class Pred, typename Is, typename... Ts>
	struct SegBucketListFilter;

	template<template<typename> class Pred, typename... Is, typename... Ts>
	struct SegBucketListFilter<Pred, TMP::List<Is...>, Ts...>
	{
		using type = TMP::ConcatT<SegBucketListFilterHelper<Pred, Ts, Is>...>;
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

	template<typename... Args>
	using BinarySegregatorAllocatorBuilderT = typename BinarySegregatorAllocatorBuilder<Args...>::type;

	template<typename... Buckets>
	struct BinarySegregatorAllocatorBuilder<TMP::List<Buckets...>>
	{
		static constexpr size_t N{ ((sizeof...(Buckets)) - 1) / 2 };

		using Center = std::tuple_element_t<N, std::tuple<Buckets...>>;
		
		using Left = typename SegBucketListFilter<
			SegBucketIndexLess<N>::Predicate, 
			TMP::IndexListFor<Buckets...>, 
			Buckets...>::type;
		
		using Right = typename SegBucketListFilter<
			SegBucketIndexGreater<N>::Predicate, 
			TMP::IndexListFor<Buckets...>, 
			Buckets...>::type;

		using type = BinarySegregatorAllocatorBuilderT<Center, Left, Right, Buckets...>;
	};

	template<size_t T, class A, typename... Ls, typename... Rs, typename... Buckets>
	struct BinarySegregatorAllocatorBuilder<SegBucket<T, A>, TMP::List<Ls...>, TMP::List<Rs...>, Buckets...>
	{
		using type = Epic::SegregatorAllocator<T,
			BinarySegregatorAllocatorBuilderT<TMP::ConcatT<TMP::List<Ls...>, TMP::List<A>>>,
			BinarySegregatorAllocatorBuilderT<TMP::List<Rs...>>>;
	};

	template<class A>
	struct BinarySegregatorAllocatorBuilder<TMP::List<A>>
	{
		using type = A;
	};

	template<size_t T, class A, class B>
	struct BinarySegregatorAllocatorBuilder<TMP::List<SegBucket<T, A>, B>>
	{
		using type = Epic::SegregatorAllocator<T, A, B>;
	};


	// MakeSegBucketList
	template<typename... Args>
	struct MakeSegBucketList;

	template<size_t T, class A, typename... Args>
	struct MakeSegBucketList<SegBucket<T, A>, Args...>
	{
		using type = TMP::ConcatT<
			TMP::List<SegBucket<T, A>>, 
			typename MakeSegBucketList<Args...>::type>;
	};

	template<size_t T, class A, typename... Args>
	struct MakeSegBucketList<TMP::Literal<size_t, T>, A, Args...>
	{
		using type = TMP::ConcatT<
			TMP::List<SegBucket<T, A>>, 
			typename MakeSegBucketList<Args...>::type>;
	};

	template<class A>
	struct MakeSegBucketList<A>
	{
		using type = TMP::List<A>;
	};


	// BinarySegregatorAllocatorBuilder Invoker
	template<typename... Args>
	using BinarySegregatorAllocatorBuilderInvoker =
		BinarySegregatorAllocatorBuilderT<typename MakeSegBucketList<Args...>::type>;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<typename... Args>
	using BinarySegregatorAllocator = typename Epic::detail::BinarySegregatorAllocatorBuilderInvoker<Args...>::type;
}
