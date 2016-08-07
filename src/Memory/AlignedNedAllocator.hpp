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
#include <cstddef>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class AlignedNedAllocator;
}

//////////////////////////////////////////////////////////////////////////////

/// AlignedNedAllocator
class Epic::AlignedNedAllocator
{
public:
	using Type = Epic::AlignedNedAllocator;

public:
	static constexpr size_t Alignment = alignof(std::max_align_t);
	static constexpr size_t MinAllocSize = 0;
	static constexpr size_t MaxAllocSize = SIZE_MAX;

public:
	constexpr AlignedNedAllocator() noexcept = default;
	constexpr AlignedNedAllocator(const Type&) noexcept = default;
	constexpr AlignedNedAllocator(Type&&) noexcept = default;

	AlignedNedAllocator& operator = (const Type&) noexcept = default;
	AlignedNedAllocator& operator = (Type&&) noexcept = default;

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	constexpr bool Owns(const Blk& blk) const noexcept
	{
		// We don't track allocated blocks and don't discriminate based on block size.
		// Therefore, we can only return true here.
		return true;
	}

public:
	/* Returns a block of uninitialized memory.
	   If sz is zero, the returned block's pointer is null. */
	Blk AllocateAligned(size_t sz, size_t alignment = Alignment) const noexcept;

	/* Delegates to ReallocateAligned(blk, sz, Alignment). */
	bool Reallocate(Blk& blk, size_t sz) const;

	/* Attempts to reallocate the memory of blk (aligned to alignment) to the new size sz.
	   If the block's pointer is null, this is equivalent to calling AllocateAligned(sz, alignment). */
	bool ReallocateAligned(Blk& blk, size_t sz, size_t alignment = Alignment) const;

public:
	/* Frees the memory for blk. */
	void DeallocateAligned(const Blk& blk) const;
};
