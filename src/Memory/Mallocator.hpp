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
	class Mallocator;
}

//////////////////////////////////////////////////////////////////////////////

/// Mallocator
class Epic::Mallocator
{
public:
	using Type = Epic::Mallocator;

public:
	static constexpr size_t Alignment = alignof(std::max_align_t);
	static constexpr size_t MinAllocSize = 0;
	static constexpr size_t MaxAllocSize = SIZE_MAX;
	static constexpr bool IsShareable = true;

public:
	constexpr Mallocator() noexcept = default;
	constexpr Mallocator(const Type&) noexcept = default;
	constexpr Mallocator(Type&&) noexcept = default;

	Mallocator& operator = (const Type&) noexcept = default;
	Mallocator& operator = (Type&&) noexcept = default;

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	constexpr bool Owns(const Blk& blk) const noexcept
	{
		// We don't track allocated blocks and don't discriminate based on block size.
		// Therefore, we can only return true here.
		return true;
	}

public:
	/* Returns a block of uninitialized memory (using std::malloc).
	   If sz is zero, the returned block's pointer is null. */
	Blk Allocate(size_t sz) const noexcept;

	/* Attempts to reallocate the memory of blk to the new size sz (using std::realloc)
	   If the block's pointer is null, this is equivalent to calling (std::malloc(sz))
	   If sz is zero, the returned block's pointer is malloc-implementation-specific. */
	bool Reallocate(Blk& blk, size_t sz) const;

public:
	/* Frees the memory for blk (using std::free). */
	void Deallocate(const Blk& blk);
};
