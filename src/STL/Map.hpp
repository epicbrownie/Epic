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
#include <map>
#include <unordered_map>
#include <boost/container/flat_map.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// STLMap<K, T, CompareFn, Pair, A>
	template<class K, 
			 class T, 
			 class CompareFn = std::less<K>, 
			 class Pair = std::pair<const K, T>, 
			 class A = Epic::DefaultAllocatorFor<Pair, eAllocatorFor::Map>>
	using STLMap = std::map<K, T, CompareFn, Epic::STLAllocator<Pair, A>>;

	/// STLMultiMap<K, T, CompareFn, Pair, A>
	template<class K, 
			 class T, 
			 class CompareFn = std::less<K>, 
			 class Pair = std::pair<const K, T>, 
			 class A = Epic::DefaultAllocatorFor<Pair, eAllocatorFor::Map>>
	using STLMultiMap = std::multimap<K, T, CompareFn, Epic::STLAllocator<Pair, A>>;

	/// STLUnorderedMap<K, T, HashFn, CompareFn, Pair, A>
	template<class K, 
			 class T,
			 class HashFn = std::hash<K>,
			 class CompareFn = std::equal_to<K>,
			 class Pair = std::pair<const K, T>,
			 class A = Epic::DefaultAllocatorFor<Pair, eAllocatorFor::Map>>
	using STLUnorderedMap = std::unordered_map<K, T, HashFn, CompareFn, Epic::STLAllocator<Pair, A>>;

	/// FlatMap<K, T, CompareFn, Pair, A>
	template<class K,
			 class T,
			 class CompareFn = std::less<K>,
			 class Pair = std::pair<K, T>,
			 class A = Epic::DefaultAllocatorFor<Pair, eAllocatorFor::Map>>
	using FlatMap = boost::container::flat_map<K, T, CompareFn, Epic::STLAllocator<Pair, A>>;

	/// FlatMultiMap<K, T, CompareFn, Pair, A>
	template<class K,
			 class T,
			 class CompareFn = std::less<K>,
			 class Pair = std::pair<K, T>,
			 class A = Epic::DefaultAllocatorFor<Pair, eAllocatorFor::Map>>
	using FlatMultiMap = boost::container::flat_multimap<K, T, CompareFn, Epic::STLAllocator<Pair, A>>;
}
