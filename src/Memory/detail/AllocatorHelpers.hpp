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
#include <Epic/Memory/detail/AllocatorTraits.hpp>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	static constexpr size_t DefaultAlignment = alignof(std::max_align_t);
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	constexpr bool IsGoodAlignment(size_t alignment) noexcept
	{
		return !(alignment == 0) &&				// alignment must be non-zero
			   !(alignment & (alignment - 1));	// alignment must be a power-of-two
	}

	constexpr size_t RoundToAligned(size_t sz, size_t alignment) noexcept
	{
		// Round sz up to the nearest multiple of alignment
		return ((sz + alignment - 1) / alignment) * alignment;
	}
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class T, bool Enabled = std::is_abstract_v<T>>
	struct AlignOf : std::integral_constant<size_t, alignof(T)> { };

	template<class T>
	struct AlignOf<T, false> : std::integral_constant<size_t, 0> { };
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class T>
	struct Reallocator;
}

//////////////////////////////////////////////////////////////////////////////

template<class T>
struct Epic::detail::Reallocator
{
	static inline bool ReallocateViaCopy(T& alloc, Blk& __restrict blk, size_t sz)
	{
		if constexpr (CanAllocate<T>::value)
		{
			auto newblk = alloc.Allocate(sz);
			if (!newblk)
				return false;

			if (blk)
			{
				std::memcpy(newblk.Ptr, blk.Ptr, std::min(sz, blk.Size));

				if constexpr (CanDeallocate<T>::value)
					alloc.Deallocate(blk);
			}

			blk = newblk;
			return true;
		}
		else
			return false;
	}

	static inline bool ReallocateAlignedViaCopy(T& alloc, Blk& __restrict blk, size_t sz, size_t alignment)
	{
		if constexpr (CanAllocateAligned<T>::value)
		{
			auto newblk = alloc.AllocateAligned(sz, alignment);
			if (!newblk)
				return false;

			if (blk)
			{
				std::memcpy(newblk.Ptr, blk.Ptr, std::min(sz, blk.Size));

				if constexpr (CanDeallocateAligned<T>::value)
					alloc.DeallocateAligned(blk);
			}

			blk = newblk;
			return true;
		}
		else
			return false;
	}
};
