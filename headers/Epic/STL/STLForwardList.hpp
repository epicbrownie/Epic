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
#include <Epic/STL/STLAllocator.hpp>
#include <forward_list>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T, class A = Epic::DefaultAllocatorFor<T, eDefaultAllocatorTypes::STLForwardList>>
	using STLForwardList = std::forward_list<T, Epic::STLAllocator<T, A>>;
}
