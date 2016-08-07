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
	enum class eSTLType
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
		STLWOStringStream
	};

	template<class T, eSTLType DefaultType>
	struct DefaultSTLAllocator
	{
		using Type = Epic::Mallocator;
	};

	template<class T, eSTLType DefaultType>
	using DefaultSTLAllocatorFor = typename DefaultSTLAllocator<T, DefaultType>::Type;
}
