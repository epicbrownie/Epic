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
#include <set>
#include <unordered_set>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// STLSet<K, CompareFn, A>
	template<class K, class CompareFn = std::less<K>, class A = Epic::DefaultAllocatorFor<K, eAllocatorFor::Set>>
	using STLSet = std::set<K, CompareFn, Epic::STLAllocator<K, A>>;

	/// STLMultiSet<K, CompareFn, A>
	template<class K, class CompareFn = std::less<K>, class A = Epic::DefaultAllocatorFor<K, eAllocatorFor::MultiSet>>
	using STLMultiSet = std::multiset<K, CompareFn, Epic::STLAllocator<K, A>>;

	/// STLUnorderedSet<K, HashFn, EqualFn, A>
	template<class K, 
			 class HashFn = std::hash<K>, 
			 class EqualFn = std::equal_to<K>, 
			 class A = Epic::DefaultAllocatorFor<K, eAllocatorFor::UnorderedSet>>
	using STLUnorderedSet = std::unordered_set<K, HashFn, EqualFn, Epic::STLAllocator<K, A>>;
}
