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
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		template<class Allocator,
				 template<class, size_t, size_t, size_t> class StoragePolicy,
				 size_t BlkSz,
				 size_t BlkCnt, 
				 size_t Align> 
		class HeapAllocatorImpl;

		template<class Allocator, size_t BlkSz, size_t BlkCnt, size_t Align> class StaticHeapPolicy;
		template<class Allocator, size_t BlkSz, size_t BlkCnt, size_t Align> class LinearHeapPolicy;
		template<class Allocator, size_t BlkSz, size_t BlkCnt, size_t Align> class BuddyHeapPolicy;
	}
}

//////////////////////////////////////////////////////////////////////////////

/// HeapAllocatorImpl<A, BlkSz, BlkCnt, Align, Policy>
template<class A, template<class, size_t, size_t, size_t> class Policy, size_t BlkSz, size_t BlkCnt, size_t Align>
class Epic::detail::HeapAllocatorImpl
	: private Policy<A, BlkSz, BlkCnt, Align>
{
public:
	using AllocatorType = A;
	using PolicyType = Policy<A, BlkSz, BlkCnt, Align>;
	using type = Epic::detail::HeapAllocatorImpl<A, Policy, BlkSz, BlkCnt, Align>;

	static_assert(std::is_default_constructible<PolicyType>::value, "The heap policy must be default-constructible.");

public:
	static constexpr size_t Alignment = PolicyType::Alignment;
	static constexpr size_t MinAllocSize = PolicyType::MinAllocSize;
	static constexpr size_t MaxAllocSize = PolicyType::MaxAllocSize;

	static constexpr size_t BlockSize = BlkSz;
	static constexpr size_t BlockCount = BlkCnt;

	static_assert(BlockSize > 0, "A heap's block size must be greater than zero.");

public:
	HeapAllocatorImpl()
		noexcept(std::is_nothrow_default_constructible<PolicyType>::value) = default;

	HeapAllocatorImpl(const type&) = delete;

	template<typename = std::enable_if_t<std::is_move_constructible<PolicyType>::value>>
	HeapAllocatorImpl(type&& obj)
		noexcept(std::is_nothrow_move_constructible<PolicyType>::value)
		: PolicyType{ std::move(obj) }
	{ }

	HeapAllocatorImpl& operator = (const type&) = delete;
	HeapAllocatorImpl& operator = (type&& obj) = delete;

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	constexpr bool Owns(const Blk& blk) const noexcept
	{
		return PolicyType::Owns(blk);
	}

public:
	/* Returns a block of uninitialized memory at least as big as sz. */
	template<typename = std::enable_if_t<detail::CanAllocate<PolicyType>::value>>
	Blk Allocate(size_t sz) noexcept
	{
		// Verify that the requested size is within our allowed bounds
		if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
			return{ nullptr, 0 };

		return PolicyType::Allocate(sz);
	}

	/* Attempts to reallocate the memory of blk to the new size sz. */
	template<typename = std::enable_if_t<detail::CanReallocate<PolicyType>::value>>
	bool Reallocate(Blk& blk, size_t sz)
	{
		// If the block isn't valid, delegate to Allocate
		if (!blk)
		{
			blk = detail::AllocateIf<type>::apply(*this, sz);
			return (bool)blk;
		}

		// If the requested size is zero, delegate to Deallocate
		if (sz == 0)
		{
			detail::DeallocateIf<type>::apply(*this, blk);
			return detail::CanDeallocate<type>::value;
		}

		// Verify that the new requested size is within our allowed bounds
		if (sz < MinAllocSize || sz > MaxAllocSize)
			return false;

		return PolicyType::Reallocate(blk, sz);
	}

	/* Returns a block of uninitialized memory.
	   Its size is all of the remaining memory. */
	template<typename = std::enable_if_t<detail::CanAllocateAll<PolicyType>::value>>
	Blk AllocateAll() noexcept
	{
		return PolicyType::AllocateAll();
	}

public:
	/* Reclaims blk's memory back into the heap. */
	template<typename = std::enable_if_t<detail::CanDeallocate<PolicyType>::value>>
	void Deallocate(const Blk& blk)
	{
		if (!blk) return;
		assert(Owns(blk) && "HeapAllocator::Deallocate - Attempted to free a block that was not allocated by this allocator");
		
		PolicyType::Deallocate(blk);
	}

	/* Frees all of the memory back into the heap. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<PolicyType>::value>>
	void DeallocateAll() noexcept
	{
		PolicyType::DeallocateAll();
	}
};

//////////////////////////////////////////////////////////////////////////////

/// StaticHeapPolicy<A, BlkSz, BlkCnt, Align>
template<class A, size_t BlkSz, size_t BlkCnt, size_t Align>
class Epic::detail::StaticHeapPolicy
{
	static_assert(std::is_default_constructible<A>::value, "The heap backing allocator must be default-constructible.");
	static_assert(detail::CanAllocate<A>::value || detail::CanAllocateAligned<A>::value,
		"The heap backing allocator must be able to perform allocations.");
	
public:
	using type = Epic::detail::StaticHeapPolicy<A, BlkSz, BlkCnt, Align>;
	using AllocatorType = A;

private:
	static constexpr bool IsAligned = (Align != 0) && (Align != A::Alignment);

public:
	static constexpr size_t Alignment = IsAligned ? Align : A::Alignment;
	static constexpr size_t MinAllocSize = 0;
	static constexpr size_t MaxAllocSize = BlkSz * BlkCnt;

	static_assert(detail::IsGoodAlignment(Alignment), "Invalid static heap alignment.");
	static_assert(!IsAligned || (IsAligned && detail::CanAllocateAligned<A>::value),
		"A static heap's alignment can only differ from the backing allocator's alignment if the allocator supports aligned allocations.");
	static_assert(!IsAligned || (BlkSz % Alignment) == 0, 
		"A static heap can only align if its block size is a multiple of the alignment.");

private:
	A m_Allocator;
	Blk m_Heap;
	size_t m_BlocksAvailable;

public:
	StaticHeapPolicy() noexcept(std::is_nothrow_default_constructible<A>::value)
		: m_Allocator{ }, m_Heap{ }, m_BlocksAvailable{ 0 } 
	{ }

	StaticHeapPolicy(const type&) = delete;

	template<typename = std::enable_if_t<std::is_move_constructible<A>::value>>
	StaticHeapPolicy(type&& obj) noexcept(std::is_nothrow_move_constructible<A>::value)
		: m_Allocator{ std::move(obj.m_Allocator) }, m_Heap{ }, m_BlocksAvailable{ 0 }
	{
		std::swap(m_Heap, obj.m_Heap);
		std::swap(m_BlocksAvailable, obj.m_BlocksAvailable);
	}

	StaticHeapPolicy& operator = (const type&) = delete;
	StaticHeapPolicy& operator = (type&& obj) = delete;

	~StaticHeapPolicy()
	{
		FreeHeap();
	}

private:
	void AllocateHeap()
	{
		if (IsAligned)
			m_Heap = detail::AllocateAlignedIf<A>::apply(m_Allocator, BlkSz * BlkCnt, Alignment);
		else
			m_Heap = detail::AllocateIf<A>::apply(m_Allocator, BlkSz * BlkCnt);

		m_BlocksAvailable = m_Heap ? BlkCnt : 0;
	}

	void FreeHeap()
	{
		if (IsAligned)
			detail::DeallocateAlignedIf<A>::apply(m_Allocator, m_Heap);
		else
			detail::DeallocateIf<A>::apply(m_Allocator, m_Heap);

		m_Heap = Blk{};
		m_BlocksAvailable = 0;
	}

	void* GetBlockPointer() const noexcept
	{
		const size_t used = BlkCnt - m_BlocksAvailable;

		return static_cast<void*>(reinterpret_cast<char*>(m_Heap.Ptr) + (BlkSz * used));
	}

protected:
	constexpr bool Owns(const Blk& blk) const noexcept
	{
		return m_Allocator.Owns(blk);
	}

	Blk Allocate(size_t sz) noexcept
	{
		// Verify heap memory
		if (!m_Heap) 
			AllocateHeap();

		// Verify there's enough free blocks remaining
		size_t blocksReq = (sz + BlkSz - 1) / BlkSz;
		if (blocksReq > m_BlocksAvailable)
			return{ nullptr, 0 };

		// Allocate the blocks
		Blk blk{ GetBlockPointer(), sz };
		m_BlocksAvailable -= blocksReq;

		return blk;
	}

	Blk AllocateAll() noexcept
	{
		// Verify heap memory
		if (!m_Heap)
			AllocateHeap();

		// Verify there's free blocks remaining
		if (m_BlocksAvailable == 0)
			return{ nullptr, 0 };

		// Allocate the blocks
		Blk blk{ GetBlockPointer(), m_BlocksAvailable * BlkSz };
		m_BlocksAvailable = 0;

		return blk;
	}

	void DeallocateAll() noexcept
	{
		if (m_Heap)
			m_BlocksAvailable = BlkCnt;
	}
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<size_t BlockSize, size_t BlockCount, class Allocator, size_t Alignment = 0>
	using StaticHeapAllocator = detail::HeapAllocatorImpl<Allocator, detail::StaticHeapPolicy, BlockSize, BlockCount, Alignment>;
}