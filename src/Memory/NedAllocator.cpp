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

#include "NedAllocator.hpp"
#include <cassert>
#include <cstdlib>

#ifdef REPLACE_SYSTEM_ALLOCATOR
	#undef REPLACE_SYSTEM_ALLOCATOR
#endif

#include "detail/NedMalloc/nedmalloc.h"

//////////////////////////////////////////////////////////////////////////////

using Epic::Blk;
using Epic::NedAllocator;

//////////////////////////////////////////////////////////////////////////////

Blk NedAllocator::Allocate(size_t sz) const noexcept
{
	// Verify that the requested size is within our allowed bounds
	if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
		return{ nullptr, 0 };

	// Delegate to nedmalloc
	auto p = nedalloc::nedmalloc(sz);
	if (!p)
		return{ nullptr, 0 };

	return{ p, sz };
}

bool NedAllocator::Reallocate(Blk& blk, size_t sz) const
{
	assert(Owns(blk) && "NedAllocator::Reallocate - Attempted to reallocate a block that was not allocated by this allocator");

	// The reallocated size must still fall within our allocation size restrictions
	if (sz < MinAllocSize || sz > MaxAllocSize) return false;

	// Attempt to reallocate the block
	auto p = nedalloc::nedrealloc(blk.Ptr, sz);

	// If ::realloc fails, the original buffer will remain unchanged
	if (!p) return false;

	// Replace the block's pointer and size
	blk = { p, sz };

	return true;
}

void NedAllocator::Deallocate(const Blk& blk) const
{
	if (!blk) return;

	assert(Owns(blk) && "NedAllocator::Deallocate - Attempted to free a block that was not allocated by this allocator");
	nedalloc::nedfree(blk.Ptr);
}
