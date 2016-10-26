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
#include <vector>
#include <boost/container/small_vector.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// STLVector<T, A>
	template<class T, class A = Epic::DefaultAllocatorFor<T, eAllocatorFor::Vector>>
	using STLVector = std::vector<T, Epic::STLAllocator<T, A>>;

	/// SmallVector<T, N, A>
	template<class T, size_t N, class A = Epic::DefaultAllocatorFor<T, eAllocatorFor::Vector>>
	using SmallVector = boost::container::small_vector<T, N, Epic::STLAllocator<T, A>>;
}
