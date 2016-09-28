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
#include <string>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// STLString<Char, A>
	template<class Char = std::string::value_type, class A = Epic::DefaultAllocatorFor<Char, eAllocatorFor::String>>
	using STLString = std::basic_string<Char, std::char_traits<Char>, Epic::STLAllocator<Char, A>>;

	/// STLWString<Char, A>
	template<class WChar = std::wstring::value_type, class A = Epic::DefaultAllocatorFor<WChar, eAllocatorFor::WString>>
	using STLWString = std::basic_string<WChar, std::char_traits<WChar>, Epic::STLAllocator<WChar, A>>;
}

//////////////////////////////////////////////////////////////////////////////

#if defined(EPIC_ENABLE_STL_ALIAS) || defined(EPIC_ENABLE_STRING_ALIAS)

namespace Epic
{
	using String = STLString<>;
	using WString = STLWString<>;
}

#endif
