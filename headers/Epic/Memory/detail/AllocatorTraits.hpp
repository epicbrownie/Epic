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
#include <Epic/TMP/TypeTraits.hpp>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		// CanAllocate - Tests for T::Allocate(size_t) -> Blk
		template<class T> using HasAllocate = decltype(std::declval<T>().Allocate(size_t()));
		template<class T> using CanAllocate = Epic::TMP::IsDetectedExact<Blk, HasAllocate, T>;

		// CanAllocateAligned - Tests for T::AllocateAligned(size_t, size_t) -> Blk
		template<class T> using HasAllocateAligned = decltype(std::declval<T>().AllocateAligned(size_t(), size_t()));
		template<class T> using CanAllocateAligned = Epic::TMP::IsDetectedExact<Blk, HasAllocateAligned, T>;

		// CanReallocate - Tests for T::Reallocate(Blk&, size_t) -> bool
		template<class T> using HasReallocate = decltype(std::declval<T>().Reallocate(std::declval<Blk&>(), size_t()));
		template<class T> using CanReallocate = Epic::TMP::IsDetectedExact<bool, HasReallocate, T>;

		// CanReallocateAligned - Tests for T::ReallocateAligned(Blk&, size_t, size_t) -> bool
		template<class T> using HasReallocateAligned = decltype(std::declval<T>().ReallocateAligned(std::declval<Blk&>(), size_t(), size_t()));
		template<class T> using CanReallocateAligned = Epic::TMP::IsDetectedExact<bool, HasReallocateAligned, T>;

		// CanAllocateAll - Tests for T::AllocateAll() -> Blk
		template<class T> using HasAllocateAll = decltype(std::declval<T>().AllocateAll());
		template<class T> using CanAllocateAll = Epic::TMP::IsDetectedExact<Blk, HasAllocateAll, T>;

		// CanAllocateAllAligned - Tests for T::AllocateAllAligned(size_t) -> Blk
		template<class T> using HasAllocateAllAligned = decltype(std::declval<T>().AllocateAllAligned(size_t()));
		template<class T> using CanAllocateAllAligned = Epic::TMP::IsDetectedExact<Blk, HasAllocateAllAligned, T>;

		// CanDeallocate - Tests for T::Deallocate(Blk) -> void
		template<class T> using HasDeallocate = decltype(std::declval<T>().Deallocate(Blk()));
		template<class T> using CanDeallocate = Epic::TMP::IsDetectedExact<void, HasDeallocate, T>;

		// CanDeallocateAligned - Tests for T::DeallocateAligned(Blk) -> void
		template<class T> using HasDeallocateAligned = decltype(std::declval<T>().DeallocateAligned(Blk()));
		template<class T> using CanDeallocateAligned = Epic::TMP::IsDetectedExact<void, HasDeallocateAligned, T>;

		// CanDeallocateAll - Tests for T::DeallocateAll() -> void
		template<class T> using HasDeallocateAll = decltype(std::declval<T>().DeallocateAll());
		template<class T> using CanDeallocateAll = Epic::TMP::IsDetectedExact<void, HasDeallocateAll, T>;
	}
}
