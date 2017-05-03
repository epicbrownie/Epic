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

#include <Epic/Memory/MemoryBlock.hpp>
#include <cstdint>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		// CanAllocate - Tests for T::Allocate(size_t) -> Blk
		template<class T, typename = void> struct CanAllocate : std::false_type { };
		template<class T> struct CanAllocate<T, std::void_t<decltype(std::declval<T>().Allocate(size_t()))>> 
			: std::is_same<decltype(std::declval<T>().Allocate(size_t())), Blk>::type { };

		// CanAllocateAligned - Tests for T::AllocateAligned(size_t, size_t) -> Blk
		template<class T, typename = void> struct CanAllocateAligned : std::false_type { };
		template<class T> struct CanAllocateAligned<T, std::void_t<decltype(std::declval<T>().AllocateAligned(size_t(), size_t()))>> 
			: std::is_same<decltype(std::declval<T>().AllocateAligned(size_t(), size_t())), Blk>::type { };

		// CanReallocate - Tests for T::Reallocate(Blk&, size_t) -> bool
		template<class T, typename = void> struct CanReallocate : std::false_type { };
		template<class T> struct CanReallocate<T, std::void_t<decltype(std::declval<T>().Reallocate(Blk(), size_t()))>> 
			: std::is_same<decltype(std::declval<T>().Reallocate(Blk(), size_t())), bool>::type { };

		// CanReallocateAligned - Tests for T::ReallocateAligned(Blk&, size_t, size_t) -> bool
		template<class T, typename = void> struct CanReallocateAligned : std::false_type { };
		template<class T> struct CanReallocateAligned<T, std::void_t<decltype(std::declval<T>().ReallocateAligned(Blk(), size_t(), size_t()))>> 
			: std::is_same<decltype(std::declval<T>().ReallocateAligned(Blk(), size_t(), size_t())), bool>::type { };

		// CanAllocateAll - Tests for T::AllocateAll() -> Blk
		template<class T, typename = void> struct CanAllocateAll : std::false_type { };
		template<class T> struct CanAllocateAll<T, std::void_t<decltype(std::declval<T>().AllocateAll())>> 
			: std::is_same<decltype(std::declval<T>().AllocateAll()), Blk>::type { };

		// CanAllocateAllAligned - Tests for T::AllocateAllAligned(size_t) -> Blk
		template<class T, typename = void> struct CanAllocateAllAligned : std::false_type { };
		template<class T> struct CanAllocateAllAligned<T, std::void_t<decltype(std::declval<T>().AllocateAllAligned(size_t()))>> 
			: std::is_same<decltype(std::declval<T>().AllocateAllAligned(size_t())), Blk>::type { };

		// CanDeallocate - Tests for T::Deallocate(Blk) -> void
		template<class T, typename = void> struct CanDeallocate : std::false_type { };
		template<class T> struct CanDeallocate<T, std::void_t<decltype(std::declval<T>().Deallocate(Blk()))>> 
			: std::is_same<decltype(std::declval<T>().Deallocate(Blk())), void>::type { };

		// CanDeallocateAligned - Tests for T::DeallocateAligned(Blk) -> void
		template<class T, typename = void> struct CanDeallocateAligned : std::false_type { };
		template<class T> struct CanDeallocateAligned<T, std::void_t<decltype(std::declval<T>().DeallocateAligned(Blk()))>> 
			: std::is_same<decltype(std::declval<T>().DeallocateAligned(Blk())), void>::type { };

		// CanDeallocateAll - Tests for T::DeallocateAll() -> void
		template<class T, typename = void> struct CanDeallocateAll : std::false_type { };
		template<class T> struct CanDeallocateAll<T, std::void_t<decltype(std::declval<T>().DeallocateAll())>> 
			: std::is_same<decltype(std::declval<T>().DeallocateAll()), void>::type { };

	}
}
