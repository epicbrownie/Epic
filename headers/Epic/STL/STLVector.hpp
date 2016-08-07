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
#include <vector>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T, class A = Epic::DefaultAllocatorFor<T, eDefaultAllocatorTypes::STLVector>>
	using STLVector = std::vector<T, Epic::STLAllocator<T, A>>;
}
