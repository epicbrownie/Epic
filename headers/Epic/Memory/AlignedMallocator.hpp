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
#include <malloc.h>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class AlignedMallocator;
}

//////////////////////////////////////////////////////////////////////////////

/// AlignedMallocator
class Epic::AlignedMallocator
{
public:
	using type = Epic::AlignedMallocator;

public:
	static constexpr size_t Alignment = alignof(std::max_align_t);
	static constexpr size_t MinAllocSize = 0;
	static constexpr size_t MaxAllocSize = _HEAP_MAXREQ;

public:
	constexpr AlignedMallocator() noexcept = default;
	constexpr AlignedMallocator(const AlignedMallocator&) noexcept = default;
	constexpr AlignedMallocator(AlignedMallocator&&) noexcept = default;

	AlignedMallocator& operator = (const AlignedMallocator&) noexcept = default;
	AlignedMallocator& operator = (AlignedMallocator&&) noexcept = default;

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	constexpr bool Owns(const Blk& blk) const noexcept
	{
		// We don't track allocated blocks and don't discriminate based on block size.
		// Therefore, we can only return true here.
		return true;
	}

public:
	/* Delegates to AllocateAligned(sz, Alignment). */
	Blk Allocate(size_t sz) const noexcept;

	/* Returns a block of uninitialized memory (using ::_aligned_malloc).
	   If sz is zero, the returned block's pointer is null. */
	Blk AllocateAligned(size_t sz, size_t alignment) const noexcept;

	/* Delegates to ReallocateAligned(blk, sz, Alignment).
	   Note: According to ::_realloc_malloc, it is an error to reallocate memory and change the alignment of a block. */
	bool Reallocate(Blk& blk, size_t sz) const;

	/* Attempts to reallocate the memory of blk (aligned to alignment) to the new size sz (using ::_realloc_malloc)
	   If the block's pointer is null, this is equivalent to calling AllocateAligned(sz, alignment)
	   If sz is zero, the returned block's pointer is malloc-implementation-specific.
	   Note: According to ::_realloc_malloc, it is an error to reallocate memory and change the alignment of a block. */
	bool ReallocateAligned(Blk& blk, size_t sz, size_t alignment) const;

public:
	/* Delegates to DeallocateAligned(blk). */
	void Deallocate(const Blk& blk);

	/* Frees the memory for blk (using ::_aligned_free). */
	void DeallocateAligned(const Blk& blk);
};
