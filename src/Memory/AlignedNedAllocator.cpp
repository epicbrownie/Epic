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

#include "AlignedNedAllocator.hpp"
#include "AllocatorHelpers.hpp"
#include <cassert>

#ifdef REPLACE_SYSTEM_ALLOCATOR
	#undef REPLACE_SYSTEM_ALLOCATOR
#endif

#include "NedMalloc/nedmalloc.h"

//////////////////////////////////////////////////////////////////////////////

using Epic::Blk;
using Epic::AlignedNedAllocator;

//////////////////////////////////////////////////////////////////////////////

Blk AlignedNedAllocator::Allocate(size_t sz) const noexcept
{
	return AllocateAligned(sz, Alignment);
}

Blk AlignedNedAllocator::AllocateAligned(size_t sz, size_t alignment) const noexcept
{
	// Verify that the alignment is acceptable
	if (!Epic::detail::IsGoodAlignment(alignment))
		return{ nullptr, 0 };

	// Verify that the requested size is within our allowed bounds
	if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
		return{ nullptr, 0 };

	// Delegate to _aligned_malloc
	auto p = nedalloc::nedmemalign(alignment, sz);
	if (!p)
		return{ nullptr, 0 };

	return{ p, sz };
}

bool AlignedNedAllocator::Reallocate(Blk& blk, size_t sz) const
{
	return ReallocateAligned(blk, sz, Alignment);
}

bool AlignedNedAllocator::ReallocateAligned(Blk& blk, size_t sz, size_t alignment) const
{
	return detail::AlignedReallocator<AlignedNedAllocator>::apply(*this, blk, sz, alignment);
}

void AlignedNedAllocator::Deallocate(const Blk& blk) const
{
	DeallocateAligned(blk);
}

void AlignedNedAllocator::DeallocateAligned(const Blk& blk) const
{
	assert(Owns(blk) && "AlignedNedAllocator::DeallocateAligned - Attempted to free a block that was not allocated by this allocator");
	nedalloc::nedfree(blk.Ptr);
}
