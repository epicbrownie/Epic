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
	template<class A>
	struct STLAllocatorAdaptor;

	struct STLAllocatorPrefix;
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::detail::STLAllocatorPrefix
{
	Epic::MemoryBlock::size_type Size;
};

//////////////////////////////////////////////////////////////////////////////

template<class A>
struct Epic::detail::STLAllocatorAdaptor
{
	using _affixed = Epic::AffixAllocator<A, Epic::detail::STLAllocatorPrefix>;

	using type = Epic::GlobalAllocator<_affixed>;
};

template<class A, class Tag>
struct Epic::detail::STLAllocatorAdaptor<Epic::detail::GlobalAllocatorImpl<A, Tag>>
{
	using _unwrapped = typename detail::UnwrapGlobalAllocator<A>::type;
	using _affixed = Epic::AffixAllocator<_unwrapped, Epic::detail::STLAllocatorPrefix>;

	using type = Epic::GlobalAllocator<_affixed, Tag>;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class Allocator>
	using STLAllocatorAdapted = typename detail::STLAllocatorAdaptor<Allocator>::type;
}
