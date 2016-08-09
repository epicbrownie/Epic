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

#include <Epic/Memory/detail/GlobalHelpers.hpp>
#include <Epic/Memory/GlobalAllocator.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class A>
	struct UniqueDeallocator;

	template<class T, class A, class Tag>
	struct UniqueAllocatorAdaptor;
}

//////////////////////////////////////////////////////////////////////////////

template<class A>
struct Epic::detail::UniqueDeallocator
{
	static void Deallocate(const Blk& blk)
	{
		A allocator;
		detail::DeallocateIf<A>::apply(allocator, blk);
	}

	static void DeallocateAligned(const Blk& blk)
	{
		A allocator;
		detail::DeallocateAlignedIf<A>::apply(allocator, blk);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class T, class A, class Tag>
struct Epic::detail::UniqueAllocatorAdaptor
{
	using Type = Epic::GlobalAllocator<A, Tag>;
};

template<class T, class A, class Tag, class OldTag>
struct Epic::detail::UniqueAllocatorAdaptor<T, Epic::detail::GlobalAllocatorImpl<A, OldTag>, Tag>
{	
	using _unwrapped = typename detail::UnwrapGlobalAllocator<A>::Type;

	using Type = Epic::GlobalAllocator<_unwrapped, OldTag>;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T, class Allocator, class Tag = detail::GlobalAllocatorTag>
	using UniqueAllocatorAdapted = typename detail::UniqueAllocatorAdaptor<T, Allocator, Tag>::Type;
}
