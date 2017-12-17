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
#include <cassert>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<size_t Bytes, size_t Alignment = detail::DefaultAlignment>
	class StackAllocator;
}

//////////////////////////////////////////////////////////////////////////////

/// StackAllocator<S, A>
template<size_t S, size_t A>
class Epic::StackAllocator
{
public:
	using Type = Epic::StackAllocator<S, A>;

public:
	static constexpr size_t Alignment = A;
	static constexpr size_t MinAllocSize = 0;
	static constexpr size_t MaxAllocSize = S;
	static constexpr size_t MemorySize = S;
	static constexpr bool IsShareable = false;

	static_assert(detail::IsGoodAlignment(Alignment), "Error: Invalid Alignment");

private:
	unsigned char* _pCursor;
	alignas(Alignment) unsigned char _Memory[MemorySize];

public:
	constexpr StackAllocator() noexcept 
		: _pCursor{ _Memory }, _Memory{ } 
	{ }

	StackAllocator(const Type&) = delete;
	StackAllocator(Type&& alloc) = delete;

	StackAllocator& operator = (const Type&) = delete;
	StackAllocator& operator = (Type&&) = delete;

private:
	unsigned char* _End() noexcept
	{
		return _Memory + MemorySize;
	}

	constexpr const unsigned char* _End() const noexcept
	{
		return _Memory + MemorySize;
	}

	size_t _Remaining() noexcept
	{
		return static_cast<size_t>(_End() - _pCursor);
	}

	constexpr size_t _Remaining() const noexcept
	{
		return static_cast<size_t>(_End() - _pCursor);
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	constexpr bool Owns(const Blk& blk) const noexcept
	{
		return (blk.Ptr >= _Memory && blk.Ptr < _End());
	}

public:
	/* Returns a block of uninitialized memory.
	   If sz is zero, the returned block's pointer is null. */
	Blk Allocate(size_t sz) noexcept
	{
		// Verify that the requested size is within our allowed bounds
		if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
			return{ nullptr, 0 };

		// Round the requested size up so that subsequent allocations remain aligned
		// unless the requested size is all of the remaining memory
		const auto szrem = _Remaining();
		const auto sznew = (sz == szrem) ? szrem : detail::RoundToAligned(sz, Alignment);
		
		// Verify that the request is not larger than available memory
		if (sznew > szrem)
			return{ nullptr, 0 };

		// Adjust cursor and return the allocation
		Blk result = { _pCursor, sz };
		_pCursor += sznew;

		return result;
	}

	/* Returns a block of uninitialized memory.
	   Its size is all of the remaining memory. */
	Blk AllocateAll() noexcept
	{
		// Calculate remaining space
		const size_t sz = _Remaining();
		
		// Verify that there's memory left to allocate
		if (sz == 0) 
			return{ nullptr, 0 };

		// Adjust cursor and return the allocation
		Blk result = { _pCursor, sz };
		_pCursor = _End();

		return result;
	}

public:
	/* If blk was the last allocated block, it will be freed.
	   Otherwise, no memory will be reclaimed. */
	void Deallocate(const Blk& blk)
	{
		if (!blk) return;

		assert(Owns(blk) && "StackAllocator::Deallocate - Attempted to free a block that was not allocated by this allocator");
		
		// If this block was the last block available (obtained via AllocateAll()), free it
		if (reinterpret_cast<unsigned char*>(blk.Ptr) + blk.Size == _End())
		{
			_pCursor = reinterpret_cast<unsigned char*>(blk.Ptr);
			return;
		}

		// Calculate the actual size of the block
		const size_t sz = detail::RoundToAligned(blk.Size, Alignment);

		// If this block was the last allocated block, free it
		if (reinterpret_cast<unsigned char*>(blk.Ptr) + sz == _pCursor)
			_pCursor = reinterpret_cast<unsigned char*>(blk.Ptr);
	}

	/* Free all of this allocator's memory */
	void DeallocateAll() noexcept
	{
		_pCursor = _Memory;
	}

private:
	void* operator new (size_t) noexcept = delete;
	void* operator new[] (size_t) noexcept = delete;
	void operator delete (void*) noexcept = delete;
	void operator delete[] (void*) noexcept = delete;
};
