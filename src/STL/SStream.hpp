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
#include <sstream>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// STLStringStream<Char, A>
	template<class Char = char, class A = Epic::DefaultAllocatorFor<Char, eAllocatorFor::StringStream>>
	using STLStringStream = std::basic_stringstream<Char, std::char_traits<Char>, Epic::STLAllocator<Char, A>>;

	/// STLIStringStream<Char, A>
	template<class Char = char, class A = Epic::DefaultAllocatorFor<Char, eAllocatorFor::IStringStream>>
	using STLIStringStream = std::basic_istringstream<Char, std::char_traits<Char>, Epic::STLAllocator<Char, A>>;

	/// STLOStringStream<Char, A>
	template<class Char = char, class A = Epic::DefaultAllocatorFor<Char, eAllocatorFor::OStringStream>>
	using STLOStringStream = std::basic_ostringstream<Char, std::char_traits<Char>, Epic::STLAllocator<Char, A>>;
}

//////////////////////////////////////////////////////////////////////////////

#if defined(EPIC_ENABLE_STL_ALIAS) || defined(EPIC_ENABLE_SSTREAM_ALIAS)

namespace Epic
{
	using StringStream = STLStringStream<char>;
	using IStringStream = STLIStringStream<char>;
	using OStringStream = STLOStringStream<char>;
	using WStringStream = STLStringStream<wchar_t>;
	using WIStringStream = STLIStringStream<wchar_t>;
	using WOStringStream = STLOStringStream<wchar_t>;
}

#endif
