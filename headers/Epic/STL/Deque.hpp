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

#include <Epic/STL/Default.hpp>
#include <Epic/STL/Allocator.hpp>
#include <deque>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// STLDeque<T, A>
	template<class T, class A = Epic::DefaultAllocatorFor<T, eAllocatorFor::STLDeque>>
	using STLDeque = std::deque<T, Epic::STLAllocator<T, A>>;
}
