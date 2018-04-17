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
#include <Epic/TMP/Utility.hpp>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<typename...>
	struct LinearSegregatorAllocatorBuilder;

	template<typename... Args>
	using LinearSegregatorAllocatorBuilderT = typename LinearSegregatorAllocatorBuilder<Args...>::type;

	template<size_t T, class S, class L>
	struct LinearSegregatorAllocatorBuilder<TMP::Literal<size_t, T>, S, L>
	{
		using type = Epic::SegregatorAllocator<T, S, L>;
	};

	template<size_t T, class S, typename... Args>
	struct LinearSegregatorAllocatorBuilder<TMP::Literal<size_t, T>, S, Args...>
	{
		using type = Epic::SegregatorAllocator<T, S, LinearSegregatorAllocatorBuilderT<Args...>>;
	};

	template<size_t T, class S, class L>
	struct LinearSegregatorAllocatorBuilder<Epic::SegBucket<T, S>, L>
	{
		using type = Epic::SegregatorAllocator<T, S, L>;
	};

	template<size_t T, class S, typename... Args>
	struct LinearSegregatorAllocatorBuilder<Epic::SegBucket<T, S>, Args...>
	{
		using type = Epic::SegregatorAllocator<T, S, LinearSegregatorAllocatorBuilderT<Args...>>;
	};
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<typename... Args>
	using LinearSegregatorAllocator = Epic::detail::LinearSegregatorAllocatorBuilderT<Args...>;
}
