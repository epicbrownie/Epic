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
#include <Epic/STL/Vector.hpp>
#include <queue>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// STLQueue<T, C>
	template<class T, class C = Epic::STLDeque<T>>
	using STLQueue = std::queue<T, C>;

	/// STLPriorityQueue<T, C, CompareFn>
	template<class T, class C = Epic::STLVector<T>, class CompareFn = std::less<typename C::value_type>>
	using STLPriorityQueue = std::priority_queue<T, C, CompareFn>;
}
