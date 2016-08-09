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

#include <Epic/Memory/GlobalAllocator.hpp>
#include <Epic/Memory/AffixAllocator.hpp>
#include <Epic/Memory/MemoryBlock.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class A, class Tag>
	struct STLAllocatorAdaptor;

	struct STLAllocatorPrefix;
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::detail::STLAllocatorPrefix
{
	Epic::MemoryBlock::size_type Size;
};

//////////////////////////////////////////////////////////////////////////////

template<class A, class Tag>
struct Epic::detail::STLAllocatorAdaptor
{
	using _affixed = Epic::AffixAllocator<A, Epic::detail::STLAllocatorPrefix>;

	using Type = Epic::GlobalAllocator<_affixed, Tag>;
};

template<class A, class Tag, class OldTag>
struct Epic::detail::STLAllocatorAdaptor<Epic::detail::GlobalAllocatorImpl<A, OldTag>, Tag>
{
	using _unwrapped = typename detail::UnwrapGlobalAllocator<A>::Type;
	using _affixed = Epic::AffixAllocator<_unwrapped, Epic::detail::STLAllocatorPrefix>;
	
	using Type = Epic::GlobalAllocator<_affixed, OldTag>;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class Allocator, class Tag = detail::GlobalAllocatorTag>
	using STLAllocatorAdapted = typename detail::STLAllocatorAdaptor<Allocator, Tag>::Type;
}
