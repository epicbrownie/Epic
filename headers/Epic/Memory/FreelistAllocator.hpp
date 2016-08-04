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

		template<class Allocator, size_t BatchSize, size_t BatchLimit, size_t MaxAllocationSize, size_t MinAllocationSize = 0, size_t DefaultAlignment = detail::DefaultAlignment>
		class FreelistAllocatorImpl;
	}
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::detail::FreelistBlock 
{ 
	FreelistBlock* pNext; 
};

//////////////////////////////////////////////////////////////////////////////

/// FreelistAllocatorImpl<A, ChunkSize, ChunkLimit, Max, Min, DefAlign>
template<class A, size_t ChunkSize, size_t ChunkLimit, size_t Max, size_t Min, size_t DefAlign>
class Epic::detail::FreelistAllocatorImpl
{
	static_assert(std::is_default_constructible<A>::value, "The freelist backing allocator must be default-constructible.");
	static_assert(detail::CanAllocate<A>::value || detail::CanAllocateAligned<A>::value, 
		"The freelist backing allocator must be able to perform allocations.");

public:
	using type = Epic::detail::FreelistAllocatorImpl<A, ChunkSize, ChunkLimit, Max, Min, DefAlign>;
	using AllocatorType = A;

private:
	struct PoolChunk { Blk Mem; PoolChunk* pNext; };

public:
	static constexpr size_t Alignment = DefAlign;
	static constexpr size_t MinAllocSize = std::max(sizeof(FreelistBlock), Min);
	static constexpr size_t MaxAllocSize = std::max(MinAllocSize, Max);

	static constexpr size_t BatchSize = ChunkSize;
	static constexpr size_t BatchLimit = ChunkLimit;

	static_assert(detail::IsGoodAlignment(Alignment), "Error: Invalid Alignment");

public:
	FreelistAllocatorImpl() noexcept(std::is_nothrow_default_constructible<A>::value)
		: m_Allocator{ }, m_pChunks{ nullptr }, m_pFreeList{ nullptr }, m_ChunkCount{ 0 }
	{ }

	FreelistAllocatorImpl(const type&) = delete;
	
	template<typename = std::enable_if_t<std::is_move_constructible<A>::value>>
	FreelistAllocatorImpl(type&& obj) noexcept(std::is_nothrow_move_constructible<A>::value)
		: m_Allocator{ std::move(obj) }, 
		  m_pChunks{ nullptr }, m_pFreeList{ nullptr }, m_ChunkCount{ 0 }
	{
		std::swap(m_pChunks, obj.m_pChunks);
		std::swap(m_pFreeList, obj.m_pFreeList);
		std::swap(m_ChunkCount, obj.m_ChunkCount);
	}

	FreelistAllocatorImpl& operator = (const type&) = delete;

	template<typename = std::enable_if_t<std::is_move_assignable<A>::value>>
	FreelistAllocatorImpl& operator = (type&& obj) noexcept(std::is_nothrow_move_assignable<A>::value)
	{
		FreeChunks();

		m_Allocator = std::move(obj.m_Allocator);

		std::swap(m_pChunks, obj.m_pChunks);
		std::swap(m_pFreeList, obj.m_pFreeList);
		std::swap(m_ChunkCount, obj.m_ChunkCount);

		return *this;
	}

	~FreelistAllocatorImpl()
	{
		FreeChunks();
	}

private:
	bool AllocateChunk() noexcept
	{
		// Verify a new chunk can be allocated
		if (m_ChunkCount == ChunkLimit)
			return false;

		// Allocate a chunk of memory from the backing allocator
		size_t freeSize = ChunkSize * MaxAllocSize;
		size_t chunkSize = freeSize + sizeof(PoolChunk) + Alignment;

		Blk chunk;

		if (detail::CanAllocate<A>::value)
			chunk = detail::AllocateIf<A>::apply(m_Allocator, chunkSize);
		else
			chunk = detail::AllocateAlignedIf<A>::apply(m_Allocator, chunkSize);

		if (!chunk) 
			return false;

		// Embed management info into the chunk
		PoolChunk* pNewChunk = new(chunk.Ptr) PoolChunk;
		pNewChunk->Mem = chunk;
		pNewChunk->pNext = m_pChunks;

		m_pChunks = pNewChunk;
		++m_ChunkCount;

		// Align the chunk
		size_t space = chunkSize - sizeof(PoolChunk);
		void* cursor = reinterpret_cast<char*>(chunk.Ptr) + sizeof(PoolChunk);
		void* pAligned = std::align(Alignment, freeSize, cursor, space);

		// Alignment should never fail
		assert(pAligned && "FreelistAllocator failed to align chunk memory.");

		// Break the chunk into free blocks and add them to the freelist
		char* pFreeBlock = reinterpret_cast<char*>(pAligned);
		
		for (size_t i = 0; i < ChunkSize; ++i)
		{
			FreelistBlock* pNewBlock = new(pFreeBlock) FreelistBlock;
			pNewBlock->pNext = m_pFreeList;
			m_pFreeList = pNewBlock;

			pFreeBlock += MaxAllocSize;
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

		m_ChunkCount = 0;
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

private:
	AllocatorType m_Allocator;
	PoolChunk* m_pChunks;
	FreelistBlock* m_pFreeList;
	size_t m_ChunkCount;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// FreelistAllocator<Allocator, BatchSize, BatchLimit, MaxAllocationSize, MinAllocationSize, DefaultAlignment>
	template<class Allocator, size_t BatchSize, size_t BatchLimit, size_t MaxAllocationSize, size_t MinAllocationSize = 0, size_t DefaultAlignment = detail::DefaultAlignment>
	using FreelistAllocator = 
		detail::FreelistAllocatorImpl
		<
			Allocator,
			std::max<size_t>(BatchSize, 1),
			BatchLimit,
			std::max(MaxAllocationSize, std::max(sizeof(detail::FreelistBlock), MinAllocationSize)),
			std::max(sizeof(detail::FreelistBlock), MinAllocationSize),
			DefaultAlignment
		>;
}
