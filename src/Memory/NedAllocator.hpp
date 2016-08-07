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
	class NedAllocator;
}

//////////////////////////////////////////////////////////////////////////////

/// NedAllocator
class Epic::NedAllocator
{
public:
	using type = Epic::NedAllocator;

public:
	static constexpr size_t Alignment = alignof(std::max_align_t);
	static constexpr size_t MinAllocSize = 0;
	static constexpr size_t MaxAllocSize = SIZE_MAX;

public:
	constexpr NedAllocator() noexcept = default;
	constexpr NedAllocator(const NedAllocator&) noexcept = default;
	constexpr NedAllocator(NedAllocator&&) noexcept = default;

	NedAllocator& operator = (const NedAllocator&) noexcept = default;
	NedAllocator& operator = (NedAllocator&&) noexcept = default;

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
	Blk Allocate(size_t sz) const noexcept;

	/* Attempts to reallocate the memory of blk to the new size sz.
	   If the block's pointer is null, this is equivalent to calling Allocate(sz). */
	bool Reallocate(Blk& blk, size_t sz) const;

public:
	/* Frees the memory for blk (using std::free). */
	void Deallocate(const Blk& blk) const;
};
