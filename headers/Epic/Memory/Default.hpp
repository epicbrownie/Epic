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

#include <Epic/Memory/Mallocator.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	enum class eDefaultAllocatorTypes
	{
		STLVector,
		STLList,
		STLMap
	};

	template<class T, eDefaultAllocatorTypes DefaultType>
	struct DefaultAllocator
	{
		using Type = Epic::Mallocator;
	};

	template<class T, eDefaultAllocatorTypes DefaultType>
	using DefaultAllocatorFor = typename DefaultAllocator<T, DefaultType>::Type;
}
