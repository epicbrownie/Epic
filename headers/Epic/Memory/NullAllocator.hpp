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

#include <Epic/Memory/detail/AllocatorHelpers.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <cstdint>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class NullAllocator;
}

//////////////////////////////////////////////////////////////////////////////

/// NullAllocator
class Epic::NullAllocator
{
public:
	using type = Epic::NullAllocator;

public:
	constexpr NullAllocator() noexcept = default;
	constexpr NullAllocator(const NullAllocator&) noexcept = default;
	constexpr NullAllocator(NullAllocator&&) noexcept = default;

	NullAllocator& operator = (const NullAllocator&) noexcept = default;
	NullAllocator& operator = (NullAllocator&&) noexcept = default;

public:
	static constexpr size_t Alignment = detail::DefaultAlignment; 
	static constexpr size_t MinAllocSize = 0;
	static constexpr size_t MaxAllocSize = SIZE_MAX;

public:
	inline bool Owns(const Blk blk) const noexcept
	{
		// blk is owned iff its pointer is null and its size is 0
		return (blk.Ptr == nullptr) && (blk.Size == 0); 
	}

public:
	Blk Allocate(size_t sz) const noexcept;
	Blk AllocateAligned(size_t sz, size_t alignment = Alignment) const noexcept;
	bool Reallocate(Blk& blk, size_t sz) const;
	bool ReallocateAligned(Blk& blk, size_t sz, size_t alignment = Alignment) const;
	Blk AllocateAll() const noexcept;
	Blk AllocateAllAligned(size_t alignment = Alignment) const noexcept;

public:
	void Deallocate(Blk blk) const;
	void DeallocateAligned(Blk blk) const;
	void DeallocateAll() const noexcept;
};
