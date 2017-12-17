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
#include <Epic/TMP/Utility.hpp>
#include <Epic/NullMutex.hpp>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <mutex>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		struct FreelistBlock;

		template<class Allocator, bool IsShared, size_t BatchSize, size_t BlockSize, size_t MinAllocationSize = 0, size_t Align = 0>
		class FreelistAllocatorImpl;
	}
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::detail::FreelistBlock 
{ 
	FreelistBlock* pNext; 
};

//////////////////////////////////////////////////////////////////////////////

/// FreelistAllocatorImpl<A, IsShared, BatchSz, Max, Min, Align>
template<class A, bool IsShared, size_t BatchSz, size_t Max, size_t Min, size_t Align>
class Epic::detail::FreelistAllocatorImpl
{
	static_assert(std::is_default_constructible<A>::value, "The freelist backing allocator must be default-constructible.");
	static_assert(detail::CanAllocate<A>::value || detail::CanAllocateAligned<A>::value, 
		"The freelist backing allocator must be able to perform allocations.");
	
public:
	using Type = Epic::detail::FreelistAllocatorImpl<A, IsShared, BatchSz, Max, Min>;
	using AllocatorType = A;

private:
	struct PoolChunk 
	{ 
		Blk Mem; 
		PoolChunk* pNext; 
	};

	static constexpr bool IsAligned = (Align != 0) && (Align != A::Alignment);

public:
	static constexpr size_t Alignment = IsAligned ? Align : A::Alignment;
	static constexpr size_t MinAllocSize = Min;
	static constexpr size_t MaxAllocSize = std::max(MinAllocSize, Max);
	static constexpr bool IsShareable = IsShared;

	static constexpr size_t BlockSize = MaxAllocSize;

	static_assert(detail::IsGoodAlignment(Alignment), "Invalid freelist alignment.");
	static_assert(!IsAligned || (IsAligned && detail::CanAllocateAligned<A>::value),
		"A freelist's alignment can only differ from the backing allocator's alignment if the allocator supports aligned allocations.");
	static_assert(!IsAligned || (BlockSize % Alignment) == 0,
		"A freelist can only align if its block size is a multiple of the alignment.");

private:
	static constexpr size_t ChunkInfoBlocks = (sizeof(PoolChunk) + BlockSize - 1) / BlockSize;

public:
	static constexpr size_t BatchSize = std::max(BatchSz, ChunkInfoBlocks + 1);

private:
	static constexpr size_t ChunkSize = BatchSize * BlockSize;

private:
	using MutexType = std::conditional_t<IsShared, std::mutex, Epic::NullMutex>;

private:
	AllocatorType m_Allocator;
	PoolChunk* m_pChunks;
	FreelistBlock* m_pFreeList;
	mutable MutexType m_Mutex;
	
public:
	FreelistAllocatorImpl() noexcept(std::is_nothrow_default_constructible<A>::value)
		: m_Allocator{ }, m_pChunks{ nullptr }, m_pFreeList{ nullptr }  
	{ }

	FreelistAllocatorImpl(const Type&) = delete;

	/* Move constructor is disabled in a shared context if the backing allocator is not shared */
	template<typename = std::enable_if_t<std::is_move_constructible<A>::value && (!IsShared || (IsShared && A::IsShareable))>>
	FreelistAllocatorImpl(Type&& obj) noexcept(std::is_nothrow_move_constructible<A>::value)
		: m_Allocator{ std::move(obj.m_Allocator) }, m_pChunks{ nullptr }, m_pFreeList{ nullptr }
	{
		{	/* CS */
			std::lock_guard<MutexType> lock(obj.m_Mutex);

			std::swap(m_pChunks, obj.m_pChunks);
			std::swap(m_pFreeList, obj.m_pFreeList);
		}
	}

	FreelistAllocatorImpl& operator = (const Type&) = delete;
	FreelistAllocatorImpl& operator = (Type&& obj) = delete;

	~FreelistAllocatorImpl()
	{
		FreeChunks();
	}

private:
	bool AllocateChunk() noexcept
	{
		// Allocate a chunk of memory from the backing allocator
		Blk chunk;

		if constexpr (IsAligned && detail::CanAllocateAligned<A>::value)
			chunk = m_Allocator.AllocateAligned(ChunkSize, Alignment);
		
		else if constexpr (!IsAligned && detail::CanAllocate<A>::value)
			chunk = m_Allocator.Allocate(ChunkSize);

		if (!chunk) 
			return false;

		// Embed management info into the chunk (at the beginning)
		PoolChunk* pNewChunk = new(chunk.Ptr) PoolChunk;
		pNewChunk->Mem = chunk;
		pNewChunk->pNext = m_pChunks;
		m_pChunks = pNewChunk;

		// Break the remaining chunk space into free blocks and add them to the freelist
		constexpr size_t remainingBlocks = BatchSize - ChunkInfoBlocks;
		auto pFreeBlocks = reinterpret_cast<unsigned char*>(chunk.Ptr) + (ChunkInfoBlocks * BlockSize);
		
		for (size_t i = 0; i < remainingBlocks; ++i)
		{
			FreelistBlock* pNewBlock = new(pFreeBlocks) FreelistBlock;
			pNewBlock->pNext = m_pFreeList;
			m_pFreeList = pNewBlock;

			pFreeBlocks += BlockSize;
		}

		return true;
	}

	void FreeChunks()
	{
		if constexpr (detail::CanDeallocateAll<A>::value)
		{
			m_Allocator.DeallocateAll();
			m_pChunks = nullptr;
		}
		else
		{
			while (m_pChunks)
			{
				auto pNext = m_pChunks->pNext;

				if constexpr (IsAligned)
				{
					if constexpr (detail::CanDeallocateAligned<A>::value)
						m_Allocator.DeallocateAligned(m_pChunks->Mem);
				}
				else
				{
					if constexpr (detail::CanDeallocate<A>::value)
						m_Allocator.Deallocate(m_pChunks->Mem);
				}

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
		Blk result{ m_pFreeList, BlockSize };
		m_pFreeList = m_pFreeList->pNext;

		return result;
	}

	void PushBlock(const Blk& blk) noexcept
	{
		if (!blk) return;

		// If the blk was allocated aligned, the alignment must be removed
		const size_t alignPad = BlockSize - blk.Size;
		auto pPtr = reinterpret_cast<unsigned char*>(blk.Ptr) - alignPad;

		// Push the adjusted pointer
		auto pNewHead = reinterpret_cast<FreelistBlock*>(pPtr);
		pNewHead->pNext = m_pFreeList;
		m_pFreeList = pNewHead;
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	bool Owns(const Blk& blk) const noexcept
	{
		/* CS */
		std::lock_guard<MutexType> lock(m_Mutex);
		return _Owns(blk);
	}

private:
	/* Non-locking Owns */
	bool _Owns(const Blk& blk) const noexcept
	{
		const PoolChunk* pChunk = m_pChunks;

		while (pChunk)
		{
			auto pEnd = static_cast<const void*>(reinterpret_cast<const unsigned char*>(pChunk->Mem.Ptr) + pChunk->Mem.Size));

			if (blk.Ptr >= pChunk->Mem.Ptr && blk.Ptr < pEnd)
				return true;

			pChunk = pChunk->pNext;
		}

		return false;
	}

public:
	/* Returns a block of uninitialized memory at least as big as sz.
	   If sz is zero, the returned block's pointer is null. */
	Blk Allocate(size_t sz) noexcept
	{
		// Verify that the requested size is within our allowed bounds
		if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
			return{ nullptr, 0 };

		{	/* CS */
			std::lock_guard<MutexType> lock(m_Mutex);

			// Try to pop a block
			Blk result = PopBlock();

			if (!result)
			{
				AllocateChunk();
				result = PopBlock();
			}

			return result;
		}
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

		{	/* CS */
			std::lock_guard<MutexType> lock(m_Mutex);

			// Try to pop a block
			Blk result = PopBlock();

			if (!result)
			{
				AllocateChunk();
				result = PopBlock();
			}

			// Attempt to calculate an aligned pointer within the block.
			// NOTE: std::align will fail for invalid blocks, so no need to validate it
			size_t space = result.Size;
			void* pAligned = result.Ptr;

			if (std::align(alignment, sz, pAligned, space))
			{
				// Alignment succeeded
				result = { pAligned, space };
			}
			else
			{
				// Alignment failed; return the block to the freelist.
				PushBlock(result);
				result = { nullptr, 0 };
			}

			return result;
		}
	}

public:
	/* Reclaims blk's memory back into the freelist. */
	void Deallocate(const Blk& blk)
	{
		if (!blk) return;

		{	/* CS */
			std::lock_guard<MutexType> lock(m_Mutex);

			assert(_Owns(blk) && "FreelistAllocator::Deallocate - Attempted to free a block that was not allocated by this allocator");
			PushBlock(blk);
		}
	}

	/* Reclaims blk's memory back into the freelist. */
	void DeallocateAligned(const Blk& blk)
	{
		if (!blk) return;

		{	/* CS */
			std::lock_guard<MutexType> lock(m_Mutex);

			assert(_Owns(blk) && "FreelistAllocator::DeallocateAligned - Attempted to free a block that was not allocated by this allocator");
			PushBlock(blk);
		}
	}

	/* Frees all of this allocator's memory */
	void DeallocateAll() noexcept
	{
		{	/* CS */
			std::lock_guard<MutexType> lock(m_Mutex);

			FreeChunks();
		}
	}
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// FreelistAllocator<Allocator, BatchSize, BlockSize, MinAllocationSize>
	template<class Allocator, size_t BatchSize, size_t BlockSize, size_t MinAllocationSize = 0>
	using FreelistAllocator = 
		detail::FreelistAllocatorImpl<Allocator, false, BatchSize,
			Epic::TMP::StaticMax<BlockSize, sizeof(detail::FreelistBlock), MinAllocationSize>::value,
			MinAllocationSize,
			0>;

	/// SharedFreelistAllocator<Allocator, BatchSize, BlockSize, MinAllocationSize>
	template<class Allocator, size_t BatchSize, size_t BlockSize, size_t MinAllocationSize = 0>
	using SharedFreelistAllocator = 
		detail::FreelistAllocatorImpl<Allocator, true, BatchSize,
			Epic::TMP::StaticMax<BlockSize, sizeof(detail::FreelistBlock), MinAllocationSize>::value,
			MinAllocationSize,
			0>;

	/// AlignedFreelistAllocator<Allocator, false, BatchSize, BlockSize, Alignment, MinAllocationSize>
	template<class Allocator, size_t BatchSize, size_t BlockSize, size_t Alignment = 0, size_t MinAllocationSize = 0>
	using AlignedFreelistAllocator =
		detail::FreelistAllocatorImpl<Allocator, false, BatchSize,
			detail::RoundToAligned(
				Epic::TMP::StaticMax<BlockSize, sizeof(detail::FreelistBlock), MinAllocationSize>::value, 
				(Alignment == 0) ? Allocator::Alignment : Alignment),
			MinAllocationSize,
			Alignment>;

	/// SharedAlignedFreelistAllocator<Allocator, false, BatchSize, BlockSize, Alignment, MinAllocationSize>
	template<class Allocator, size_t BatchSize, size_t BlockSize, size_t Alignment = 0, size_t MinAllocationSize = 0>
	using SharedAlignedFreelistAllocator =
		detail::FreelistAllocatorImpl<Allocator, true, BatchSize,
			detail::RoundToAligned(
				Epic::TMP::StaticMax<BlockSize, sizeof(detail::FreelistBlock), MinAllocationSize>::value, 
				(Alignment == 0) ? Allocator::Alignment : Alignment),
			MinAllocationSize,
			Alignment>;
}
