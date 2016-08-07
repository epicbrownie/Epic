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

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class Allocator, class Tag>
	class GlobalAllocatorImpl;

	namespace detail
	{
		template<class A> 
		struct UnwrapGlobalAllocator;
	}
}

//////////////////////////////////////////////////////////////////////////////

template<class A>
struct Epic::detail::UnwrapGlobalAllocator
{
	using Type = A;
};

template<class A, class Tag>
struct Epic::detail::UnwrapGlobalAllocator<Epic::GlobalAllocatorImpl<A, Tag>>
{
	using Type = typename UnwrapGlobalAllocator<A>::Type;
};
