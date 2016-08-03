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

#include "Mallocator.hpp"
#include <cassert>
#include <cstdlib>

//////////////////////////////////////////////////////////////////////////////

using Epic::Blk;
using Epic::Mallocator;

//////////////////////////////////////////////////////////////////////////////

Blk Mallocator::Allocate(size_t sz) const noexcept
{
	// Verify that the requested size is within our allowed bounds
	if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
		return{ nullptr, 0 };

	// Delegate to malloc
	auto p = std::malloc(sz);
	if (!p)
		return{ nullptr, 0 };

	return{ p, sz };
}

bool Mallocator::Reallocate(Blk& blk, size_t sz) const
{
	assert(Owns(blk) && "Mallocator::Reallocate - Attempted to reallocate a block that was not allocated by this allocator");

	// The reallocated size must still fall within our allocation size restrictions
	if (sz < MinAllocSize || sz > MaxAllocSize) return false;

	// Attempt to reallocate the block
	auto p = std::realloc(blk.Ptr, sz);

	// If ::realloc fails, the original buffer will remain unchanged
	if (!p) return false;

	// Replace the block's pointer and size
	blk = { p, sz };

	return true;
}

void Mallocator::Deallocate(const Blk& blk)
{
	if (!blk) return;

	assert(Owns(blk) && "Mallocator::Deallocate - Attempted to free a block that was not allocated by this allocator");
	std::free(blk.Ptr);
}
