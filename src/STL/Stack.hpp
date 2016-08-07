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
#include <Epic/STL/Deque.hpp>
#include <stack>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// STLStack<T, C>
	template<class T, class C = Epic::STLDeque<T>>
	using STLStack = std::stack<T, C>;
}
