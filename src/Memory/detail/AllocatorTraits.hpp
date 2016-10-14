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
		template<class T> using AllocateExpr = decltype(std::declval<T>().Allocate(size_t()));
		template<class T> using CanAllocate = Epic::TMP::IsValidExpr<T, Blk, AllocateExpr>;

		// CanAllocateAligned - Tests for T::AllocateAligned(size_t, size_t) -> Blk
		template<class T> using AllocateAlignedExpr = decltype(std::declval<T>().AllocateAligned(size_t(), size_t()));
		template<class T> using CanAllocateAligned = Epic::TMP::IsValidExpr<T, Blk, AllocateAlignedExpr>;

		// CanReallocate - Tests for T::Reallocate(Blk&, size_t) -> bool
		template<class T> using ReallocateExpr = decltype(std::declval<T>().Reallocate(Blk(), size_t()));
		template<class T> using CanReallocate = Epic::TMP::IsValidExpr<T, bool, ReallocateExpr>;

		// CanReallocateAligned - Tests for T::ReallocateAligned(Blk&, size_t, size_t) -> bool
		template<class T> using ReallocateAlignedExpr = decltype(std::declval<T>().ReallocateAligned(Blk(), size_t(), size_t()));
		template<class T> using CanReallocateAligned = Epic::TMP::IsValidExpr<T, bool, ReallocateAlignedExpr>;

		// CanAllocateAll - Tests for T::AllocateAll() -> Blk
		template<class T> using AllocateAllExpr = decltype(std::declval<T>().AllocateAll());
		template<class T> using CanAllocateAll = Epic::TMP::IsValidExpr<T, Blk, AllocateAllExpr>;

		// CanAllocateAllAligned - Tests for T::AllocateAllAligned(size_t) -> Blk
		template<class T> using AllocateAllAlignedExpr = decltype(std::declval<T>().AllocateAllAligned(size_t()));
		template<class T> using CanAllocateAllAligned = Epic::TMP::IsValidExpr<T, Blk, AllocateAllAlignedExpr>;

		// CanDeallocate - Tests for T::Deallocate(Blk) -> void
		template<class T> using DeallocateExpr = decltype(std::declval<T>().Deallocate(Blk()));
		template<class T> using CanDeallocate = Epic::TMP::IsValidExpr<T, void, DeallocateExpr>;

		// CanDeallocateAligned - Tests for T::DeallocateAligned(Blk) -> void
		template<class T> using DeallocateAlignedExpr = decltype(std::declval<T>().DeallocateAligned(Blk()));
		template<class T> using CanDeallocateAligned = Epic::TMP::IsValidExpr<T, void, DeallocateAlignedExpr>;

		// CanDeallocateAll - Tests for T::DeallocateAll() -> void
		template<class T> using DeallocateAllExpr = decltype(std::declval<T>().DeallocateAll());
		template<class T> using CanDeallocateAll = Epic::TMP::IsValidExpr<T, void, DeallocateAllExpr>;
	}
}
