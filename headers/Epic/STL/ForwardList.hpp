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

#include <Epic/Memory/Default.hpp>
#include <Epic/STL/Allocator.hpp>
#include <forward_list>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// STLForwardList<T, A>
	template<class T, class A = Epic::DefaultAllocatorFor<T, eAllocatorFor::ForwardList>>
	using STLForwardList = std::forward_list<T, Epic::STLAllocator<T, A>>;
}
