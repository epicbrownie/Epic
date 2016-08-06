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
#include <Epic/Memory/detail/AllocatorTraits.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		struct FreelistBlock;

		template<class Allocator, size_t BatchSize, size_t MaxAllocationSize, size_t MinAllocationSize = 0>
		class FreelistAllocatorImpl;
	}
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::detail::FreelistBlock 
{ 
	FreelistBlock* pNext; 
};

//////////////////////////////////////////////////////////////////////////////

/// FreelistAllocatorImpl<A, BatchSz, Max, Min>
template<class A, size_t BatchSz, size_t Max, size_t Min>
class Epic::detail::FreelistAllocatorImpl
{
	static_assert(std::is_default_constructible<A>::value, "The freelist backing allocator must be default-constructible.");
	static_assert(detail::CanAllocate<A>::value || detail::CanAllocateAligned<A>::value, 
		"The freelist backing allocator must be able to perform allocations.");

public:
	using type = Epic::detail::FreelistAllocatorImpl<A, BatchSz, Max, Min>;
	using AllocatorType = A;

private:
	struct PoolChunk { Blk Mem; PoolChunk* pNext; };

public:
	static constexpr size_t Alignment = A::Alignment;
	static constexpr size_t MinAllocSize = Min;
	static constexpr size_t MaxAllocSize = std::max(MinAllocSize, Max);

private:
	static constexpr size_t ChunkInfoBlocks = (sizeof(PoolChunk) + MaxAllocSize - 1) / MaxAllocSize;

public:
	static constexpr size_t BatchSize = std::max(BatchSz, ChunkInfoBlocks + 1);

private:
	static constexpr size_t ChunkSize = BatchSize * MaxAllocSize;

private:
	AllocatorType m_Allocator;
	PoolChunk* m_pChunks;
	FreelistBlock* m_pFreeList;
	
public:
	FreelistAllocatorImpl() noexcept(std::is_nothrow_default_constructible<A>::value)
		: m_Allocator{ }, m_pChunks{ nullptr }, m_pFreeList{ nullptr }
	{ }

	FreelistAllocatorImpl(const type&) = delete;
	
	template<typename = std::enable_if_t<std::is_move_constructible<A>::value>>
	FreelistAllocatorImpl(type&& obj) noexcept(std::is_nothrow_move_constructible<A>::value)
		: m_Allocator{ std::move(obj) }, m_pChunks{ nullptr }, m_pFreeList{ nullptr }
	{
		std::swap(m_pChunks, obj.m_pChunks);
		std::swap(m_pFreeList, obj.m_pFreeList);
	}

	FreelistAllocatorImpl& operator = (const type&) = delete;
	FreelistAllocatorImpl& operator = (type&& obj) = delete;

	~FreelistAllocatorImpl()
	{
		FreeChunks();
	}

private:
	bool AllocateChunk() noexcept
	{
		// Allocate a chunk of memory from the backing allocator
		Blk chunk;

		if (detail::CanAllocate<A>::value)
			chunk = detail::AllocateIf<A>::apply(m_Allocator, ChunkSize);
		else
			chunk = detail::AllocateAlignedIf<A>::apply(m_Allocator, ChunkSize, Alignment);

		if (!chunk) 
			return false;

		// Embed management info into the chunk (at the beginning)
		PoolChunk* pNewChunk = new(chunk.Ptr) PoolChunk;
		pNewChunk->Mem = chunk;
		pNewChunk->pNext = m_pChunks;
		m_pChunks = pNewChunk;

		// Break the remaining chunk space into free blocks and add them to the freelist
		size_t remainingBlocks = BatchSize - ChunkInfoBlocks;
		char* pFreeBlocks = reinterpret_cast<char*>(chunk.Ptr) + (ChunkInfoBlocks * MaxAllocSize);
		
		for (size_t i = 0; i < remainingBlocks; ++i)
		{
			FreelistBlock* pNewBlock = new(pFreeBlocks) FreelistBlock;
			pNewBlock->pNext = m_pFreeList;
			m_pFreeList = pNewBlock;

			pFreeBlocks += MaxAllocSize;
		}

		return true;
	}

	void AllocateChunks(size_t count) noexcept
	{
		for (size_t i = 0; i < count; ++i)
			if (!AllocateChunk()) break;
	}

	void FreeChunks()
	{
		if (detail::CanDeallocateAll<A>::value)
		{
			// Static-If: Deallocate all chunks at once
			detail::DeallocateAllIf<A>::apply(m_Allocator);
			m_pChunks = nullptr;
		}
		else
		{
			// Static-If: Deallocate one at a time
			while (m_pChunks)
			{
				auto pNext = m_pChunks->pNext;

				if (detail::CanAllocate<A>::value)
					detail::DeallocateIf<A>::apply(m_Allocator, m_pChunks->Mem);
				else
					detail::DeallocateAlignedIf<A>::apply(m_Allocator, m_pChunks->Mem);

				m_pChunks = pNext;
			}
		}

		m_pFreeList = nullptr;
	}

	Blk PopBlock() noexcept
	{
		// Verify there's a block to pop
		if (!m_pFreeList)
			return{ nullptr, 0 };

		// Take the head block
		Blk result{ m_pFreeList, MaxAllocSize };
		m_pFreeList = m_pFreeList->pNext;

		return result;
	}

	void PushBlock(const Blk& blk) noexcept
	{
		// If the blk was allocated aligned, the alignment must be removed
		size_t alignPad = MaxAllocSize - blk.Size;
		char* pPtr = reinterpret_cast<char*>(blk.Ptr) - alignPad;

		// Push the adjusted pointer
		auto pNewHead = reinterpret_cast<FreelistBlock*>(pPtr);
		pNewHead->pNext = m_pFreeList;
		m_pFreeList = pNewHead;
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	constexpr bool Owns(const Blk& blk) const noexcept
	{
		return m_Allocator.Owns(blk);
	}

public:
	/* Returns a block of uninitialized memory at least as big as sz.
	   If sz is zero, the returned block's pointer is null. */
	Blk Allocate(size_t sz) noexcept
	{
		// Verify that the requested size is within our allowed bounds
		if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
			return{ nullptr, 0 };

		// Try to pop a block
		Blk result = PopBlock();
		
		if (!result)
		{
			AllocateChunk();
			result = PopBlock();
		}

		return result;
	}

	/* Returns a block of uninitialized memory at least as big as sz (aligned to alignment).
	   If sz is zero, the returned block's pointer is null. */
	Blk AllocateAligned(size_t sz, size_t alignment = Alignment) noexcept
	{
		// Verify that the alignment is acceptable
		if (!detail::IsGoodAlignment(alignment))
			return{ nullptr, 0 };

		// Verify that the requested size is within our allowed bounds
		if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
			return{ nullptr, 0 };

		// Allocate a full block
		Blk result = Allocate(MaxAllocSize);
		if (!result)
			return{ nullptr, 0 };

		// Attempt to calculate an aligned pointer within the block
		size_t space = result.Size;
		void* cursor = result.Ptr;

		if (std::align(alignment, sz, cursor, space))
		{
			// Alignment succeeded
			result = { cursor, space };
		}
		else
		{
			// Alignment failed; return the block to the freelist.
			PushBlock(result);
			result = { nullptr, 0 };
		}

		return result;
	}

public:
	/* Reclaims blk's memory back into the freelist. */
	void Deallocate(const Blk& blk)
	{
		if (!blk) return;
		assert(Owns(blk) && "FreelistAllocator::Deallocate - Attempted to free a block that was not allocated by this allocator");
		PushBlock(blk);
	}

	/* Reclaims blk's memory back into the freelist. */
	void DeallocateAligned(const Blk& blk)
	{
		if (!blk) return;
		assert(Owns(blk) && "FreelistAllocator::DeallocateAligned - Attempted to free a block that was not allocated by this allocator");
		PushBlock(blk);
	}

	/* Frees all of this allocator's memory */
	void DeallocateAll() noexcept
	{
		FreeChunks();
	}
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// FreelistAllocator<Allocator, BatchSize, MaxAllocationSize, MinAllocationSize>
	template<class Allocator, size_t BatchSize, size_t MaxAllocationSize, size_t MinAllocationSize = 0>
	using FreelistAllocator = 
		detail::FreelistAllocatorImpl<Allocator, BatchSize,
			std::max(MaxAllocationSize, std::max(sizeof(detail::FreelistBlock), MinAllocationSize)),
			MinAllocationSize>;

	/// AlignedFreelistAllocator<Allocator, BatchSize, MaxAllocationSize, MinAllocationSize>
	template<class Allocator, size_t BatchSize, size_t MaxAllocationSize, size_t MinAllocationSize = 0>
	using AlignedFreelistAllocator =
		detail::FreelistAllocatorImpl<Allocator, BatchSize,
			detail::RoundToAligned(std::max(MaxAllocationSize, std::max(sizeof(detail::FreelistBlock), MinAllocationSize)), Allocator::Alignment),
			MinAllocationSize>;
}
