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
	enum class eAllocatorFor
	{
		STLString,
		STLWString,

		STLList,
		STLForwardList,
		STLDeque,
		STLVector,
		STLMap,
		STLMultiSet,
		STLMultiMap,
		STLSet,
		STLUnorderedMap,
		STLUnorderedSet,

		STLStringStream,
		STLIStringStream,
		STLOStringStream,
		STLWStringStream,
		STLWIStringStream,
		STLWOStringStream,
		  
		STLSharedPtr
	};

	template<class T, eAllocatorFor DefaultForType>
	struct DefaultAllocator
	{
		using Type = Epic::Mallocator;
	};

	template<class T, eAllocatorFor DefaultForType>
	using DefaultAllocatorFor = typename DefaultAllocator<T, DefaultForType>::Type;
}
