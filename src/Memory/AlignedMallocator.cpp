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

#include "AlignedMallocator.hpp"
#include "detail/AllocatorHelpers.hpp"
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

using Epic::Blk;
using Epic::AlignedMallocator;

//////////////////////////////////////////////////////////////////////////////

Blk AlignedMallocator::AllocateAligned(size_t sz, size_t alignment) const noexcept
{
	// Verify that the alignment is acceptable
	if (!Epic::detail::IsGoodAlignment(alignment))
		return{ nullptr, 0 };

	// Verify that the requested size is within our allowed bounds
	if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
		return{ nullptr, 0 };

	// Delegate to _aligned_malloc
	auto p = ::_aligned_malloc(sz, alignment);
	if (!p)
		return{ nullptr, 0 };

	return{ p, sz };
}

bool AlignedMallocator::ReallocateAligned(Blk& blk, size_t sz, size_t alignment) const
{
	assert(Owns(blk) && "AlignedMallocator::ReallocateAligned - Attempted to reallocate a block that was not allocated by this allocator");

	// Verify that the alignment is acceptable
	if (!Epic::detail::IsGoodAlignment(alignment)) return false;

	// Verify that the requested size is within our allowed bounds
	if (sz < MinAllocSize || sz > MaxAllocSize) return false;

	// Attempt to reallocate the block
	auto p = ::_aligned_realloc(blk.Ptr, sz, alignment);

	// If _aligned_realloc fails, the original buffer will remain unchanged
	if (!p) return false;

	// Replace the block's pointer and size
	blk = { p, sz };

	return true;
}

void AlignedMallocator::DeallocateAligned(const Blk& blk)
{
	if (!blk) return;

	assert(Owns(blk) && "AlignedMallocator::DeallocateAligned - Attempted to free a block that was not allocated by this allocator");
	::_aligned_free(blk.Ptr);
}
