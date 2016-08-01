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

#include <Epic/Memory/AllocatorHelpers.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <cassert>
#include <cstdint>
#include <memory>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<size_t Bytes, size_t DefaultAlignment = detail::DefaultAlignment>
	class AlignedStackAllocator;
}

//////////////////////////////////////////////////////////////////////////////

// AlignedStackAllocator<S, A>
template<size_t S, size_t A>
class Epic::AlignedStackAllocator
{
public:
	using type = Epic::AlignedStackAllocator<S, A>;
	
public:
	static constexpr size_t Alignment = A;
	static constexpr size_t MinAllocSize = 0;
	static constexpr size_t MaxAllocSize = S;
	static constexpr size_t MemorySize = S;

	static_assert(detail::IsGoodAlignment(Alignment), "Error: Invalid default alignment");

public:
	constexpr AlignedStackAllocator() noexcept
		: _pCursor{ _Memory }, _Memory{ } 
	{ }

	AlignedStackAllocator(const AlignedStackAllocator<S, A>&) = delete;
	AlignedStackAllocator(AlignedStackAllocator<S, A>&& alloc) = delete;

	AlignedStackAllocator& operator = (const AlignedStackAllocator<S, A>&) = delete;
	AlignedStackAllocator& operator = (AlignedStackAllocator<S, A>&&) = delete;

private:
	constexpr const char* _End() const noexcept
	{
		return _Memory + MemorySize;
	}

	constexpr size_t _Remaining() const noexcept
	{
		return static_cast<size_t>(_End() - _pCursor);
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	constexpr bool Owns(const Blk& blk) const noexcept
	{
		return (blk.Ptr == nullptr) ||
			   (blk.Ptr >= _Memory && blk.Ptr < _End());
	}

public:
	/* Delegates to AllocateAligned(sz, Alignment). */
	Blk Allocate(size_t sz) noexcept
	{
		return AllocateAligned(sz, Alignment);
	}

	/* Returns a block of uninitialized memory (aligned to alignment).
	   If sz is zero, the returned block's pointer is null. */
	Blk AllocateAligned(size_t sz, size_t alignment = Alignment) noexcept
	{
		// Verify that the alignment is acceptable
		if (!detail::IsGoodAlignment(alignment))
			return{ nullptr, 0 };

		// Verify that the requested size is within our allowed bounds
		if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
			return{ nullptr, 0 };

		// Calculate an aligned pointer to available memory if there is enough space available
		Blk result;
		size_t space = _Remaining();
		void* cursor = _pCursor;
		void* pAligned = std::align(alignment, sz, cursor, space);

		// If space was available, update the result and cursor
		if (pAligned)
		{
			result = { pAligned, sz };
			_pCursor = static_cast<char*>(pAligned) + sz;
		}

		return result;
	}

	/* Delegates to AllocateAllAligned() with default alignment. */
	Blk AllocateAll() noexcept
	{
		return AllocateAllAligned(Alignment);
	}

	/* Returns a block of uninitialized memory.
	   alignment must be a non-zero power of two.
	   Its size is all of the remaining memory and it will be aligned to alignment. */
	Blk AllocateAllAligned(size_t alignment = Alignment) noexcept
	{
		// Verify that the alignment is acceptable
		if (!detail::IsGoodAlignment(alignment))
			return{ nullptr, 0 };

		// Verify that the requested size is within our allowed bounds
		size_t szavail = _Remaining();
		if (szavail == 0 || szavail < MinAllocSize || szavail > MaxAllocSize)
			return{ nullptr, 0 };

		// Attempt the allocation
		void* cursor = _pCursor;
		void* pAligned = std::align(alignment, 0, cursor, szavail);
		Blk result;

		if (pAligned)
		{
			size_t szReserved = static_cast<size_t>(_End() - static_cast<char*>(pAligned));
			result = { pAligned, szReserved };
			_pCursor = _End();
		}
		
		return result;
	}

public:
	/* Delegates to DeallocateAligned(blk). */
	void Deallocate(const Blk& blk)
	{
		DeallocateAligned(blk);
	}

	/* No individual memory allocation can be reclaimed by this allocator. */
	void DeallocateAligned(const Blk& blk)
	{
		assert(Owns(blk) && "StackAllocator::Deallocate - Attempted to free a block that was not allocated by this allocator");
	}

	/* Frees all of this allocator's memory */
	void DeallocateAll() noexcept
	{
		_pCursor = _Memory;
	}

private:
	void* operator new (size_t) noexcept = delete;
	void* operator new[] (size_t) noexcept = delete;
	void operator delete (void*) noexcept = delete;
	void operator delete[] (void*) noexcept = delete;

private:
	char* _pCursor;
	alignas(Alignment) char _Memory[MemorySize];
};
