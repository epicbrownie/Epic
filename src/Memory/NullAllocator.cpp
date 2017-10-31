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

#include "NullAllocator.hpp"
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

using Epic::Blk;
using Epic::NullAllocator;

//////////////////////////////////////////////////////////////////////////////

Blk NullAllocator::Allocate(size_t /*sz*/) const noexcept
{
	return{ nullptr, 0 };
}

Blk NullAllocator::AllocateAligned(size_t /*sz*/, size_t /*alignment*/) const noexcept
{
	return{ nullptr, 0 };
}

bool NullAllocator::Reallocate(Blk& blk, size_t /*sz*/) const
{
	assert(blk.Ptr == nullptr && "NullAllocator::Reallocate - blk.Ptr must be null");
	return true;
}

bool NullAllocator::ReallocateAligned(Blk& blk, size_t /*sz*/, size_t /*alignment*/) const
{
	assert(blk.Ptr == nullptr && "NullAllocator::ReallocateAligned - blk.Ptr must be null");
	return true;
}

Blk NullAllocator::AllocateAll() const noexcept
{
	return{ nullptr, 0 };
}

Blk NullAllocator::AllocateAllAligned(size_t /*alignment*/) const noexcept
{
	return{ nullptr, 0 };
}

void NullAllocator::Deallocate(Blk blk) const
{
	assert(blk.Ptr == nullptr && "NullAllocator::Deallocate - blk.Ptr must be null");
}

void NullAllocator::DeallocateAligned(Blk blk) const
{
	assert(blk.Ptr == nullptr && "NullAllocator::DeallocateAligned - blk.Ptr must be null");
}

void NullAllocator::DeallocateAll() const noexcept 
{

}
