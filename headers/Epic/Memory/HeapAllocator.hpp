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
#include <Epic/Memory/detail/HeapHelpers.hpp>
#include <Epic/Memory/detail/AllocatorTraits.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <cassert>
#include <cstdint>
#include <type_traits>
#include <memory>

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
		template<class Allocator, size_t BlkSz, size_t BlkCnt, size_t Align> class InternalLinearHeapPolicy;
		template<class Allocator, size_t BlkSz, size_t BlkCnt, size_t Align> class ExternalLinearHeapPolicy;
		template<class Allocator, size_t BlkSz, size_t BlkCnt, size_t Align> class BuddyHeapPolicy;

		template<class StoragePolicy, class Allocator, size_t BlkSz, size_t BlkCnt, size_t Align> class LinearHeapPolicyImpl;
		template<class Allocator, size_t BlkSz, size_t BlkCnt, size_t Align> class LinearHeapInternalStoragePolicy;
		template<class Allocator, size_t BlkSz, size_t BlkCnt, size_t Align> class LinearHeapExternalStoragePolicy;
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

	///* Attempts to reallocate the memory of blk to the new size sz. */
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
			if (detail::CanDeallocate<type>::value)
			{
				detail::DeallocateIf<type>::apply(*this, blk);
				blk = { nullptr, 0 };
			}

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

public:
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

/// LinearHeapInternalStoragePolicy<A, BlkSz, BlkCnt, Align>
template<class A, size_t BlkSz, size_t BlkCnt, size_t Align>
class Epic::detail::LinearHeapInternalStoragePolicy
{
	static_assert(detail::CanAllocate<A>::value || detail::CanAllocateAligned<A>::value,
		"The heap backing allocator must be able to perform allocations.");

public:
	using type = Epic::detail::LinearHeapInternalStoragePolicy<A, BlkSz, BlkCnt, Align>;

protected:
	static constexpr bool IsAligned = (Align != 0) && (Align != A::Alignment);

public:
	static constexpr size_t Alignment = IsAligned ? Align : A::Alignment;

	static_assert(detail::IsGoodAlignment(Alignment), "Invalid linear heap alignment.");
	static_assert(!IsAligned || (IsAligned && detail::CanAllocateAligned<A>::value),
		"A linear heap's alignment can only differ from the backing allocator's alignment if the allocator supports aligned allocations.");
	static_assert(!IsAligned || (BlkSz % Alignment) == 0,
		"A linear heap can only align if its block size is a multiple of the alignment.");

protected:
	using BitmapType = Epic::detail::HeapBitmap<BlkCnt>;
	static constexpr size_t BitmapSize = sizeof(BitmapType);

	static_assert(BlkSz * BlkCnt > BitmapSize, "This linear heap is not large enough to store management information internally.");

protected:
	Blk m_Heap;

protected:
	constexpr LinearHeapInternalStoragePolicy() noexcept = default;
	LinearHeapInternalStoragePolicy(const type&) = delete;

	constexpr LinearHeapInternalStoragePolicy(type&& obj) noexcept 
		: m_Heap{ }
	{
		std::swap(m_Heap, obj.m_Heap);
	}
	
	LinearHeapInternalStoragePolicy& operator = (const type&) = delete;
	LinearHeapInternalStoragePolicy& operator = (type&& obj) = delete;

protected:
	void AllocateHeap(A& allocator) noexcept
	{
		// Allocate heap space
		if (IsAligned)
			m_Heap = detail::AllocateAlignedIf<A>::apply(allocator, BlkSz * BlkCnt, Alignment);
		else
			m_Heap = detail::AllocateIf<A>::apply(allocator, BlkSz * BlkCnt);

		// Use some of the heap space to hold a new bitmap (always at block 0)
		if (m_Heap)
		{
			size_t blocksReq = (BitmapSize + BlkSz - 1) / BlkSz;

			auto pBitmap = new (GetBitmapPointer()) BitmapType{};
			pBitmap->Reset();

std::cout << "Fresh Heap:" << std::endl;
pBitmap->Output();
std::cout << std::endl;

			pBitmap->Set(0, blocksReq, true);

std::cout << "Placed Bitmap:" << std::endl;
pBitmap->Output();
std::cout << std::endl;
		}
	}

	void FreeHeap(A& allocator)
	{
		if (m_Heap)
			GetBitmapPointer()->~BitmapType();

		if (IsAligned)
			detail::DeallocateAlignedIf<A>::apply(allocator, m_Heap);
		else
			detail::DeallocateIf<A>::apply(allocator, m_Heap);

		m_Heap = Blk{};
	}

	constexpr BitmapType* GetBitmapPointer() const
	{
		return static_cast<BitmapType*>(m_Heap.Ptr);
	}
};

/// LinearHeapExternalStoragePolicy<A, BlkSz, BlkCnt, Align>
template<class A, size_t BlkSz, size_t BlkCnt, size_t Align>
class Epic::detail::LinearHeapExternalStoragePolicy
{
	static_assert(detail::CanAllocate<A>::value || detail::CanAllocateAligned<A>::value,
		"The heap backing allocator must be able to perform allocations.");

public:
	using type = Epic::detail::LinearHeapExternalStoragePolicy<A, BlkSz, BlkCnt, Align>;

protected:
	static constexpr bool IsAligned = (Align != 0) && (Align != A::Alignment);

public:
	static constexpr size_t Alignment = IsAligned ? Align : A::Alignment;

	static_assert(detail::IsGoodAlignment(Alignment), "Invalid linear heap alignment.");
	static_assert(!IsAligned || (IsAligned && detail::CanAllocateAligned<A>::value),
		"A linear heap's alignment can only differ from the backing allocator's alignment if the allocator supports aligned allocations.");
	static_assert(!IsAligned || (BlkSz % Alignment) == 0,
		"A linear heap can only align if its block size is a multiple of the alignment.");

protected:
	using BitmapType = Epic::detail::HeapBitmap<BlkCnt>;
	static constexpr size_t BitmapSize = sizeof(BitmapType);

protected:
	Blk m_Heap;

protected:
	constexpr LinearHeapExternalStoragePolicy() noexcept = default;
	LinearHeapExternalStoragePolicy(const type&) = delete;

	constexpr LinearHeapExternalStoragePolicy(type&& obj) noexcept
		: m_Heap{ }
	{
		std::swap(m_Heap, obj.m_Heap);
	}

	LinearHeapExternalStoragePolicy& operator = (const type&) = delete;
	LinearHeapExternalStoragePolicy& operator = (type&& obj) = delete;

protected:
	void AllocateHeap(A& allocator) noexcept
	{
		// Allocate heap space
		Blk blk;
		size_t sz = (BlkSz * BlkCnt) + BitmapSize;

		if (IsAligned)
			blk = detail::AllocateAlignedIf<A>::apply(allocator, sz, Alignment);
		else
			blk = detail::AllocateIf<A>::apply(allocator, sz);

		// Use some of the space to hold a new bitmap (after heap data)
		if (blk)
		{
			m_Heap = { blk.Ptr, BlkSz * BlkCnt };

			auto pBitmap = new (GetBitmapPointer()) BitmapType{};
			pBitmap->Reset();

std::cout << "Fresh Heap:" << std::endl;
pBitmap->Output();
std::cout << std::endl;
		}
	}

	void FreeHeap(A& allocator)
	{
		if (m_Heap)
			GetBitmapPointer()->~BitmapType();

		if (IsAligned)
			detail::DeallocateAlignedIf<A>::apply(allocator, { m_Heap.Ptr, m_Heap.Size + BitmapSize });
		else
			detail::DeallocateIf<A>::apply(allocator, { m_Heap.Ptr, m_Heap.Size + BitmapSize });

		m_Heap = Blk{};
	}

	constexpr BitmapType* GetBitmapPointer() const
	{
		return reinterpret_cast<BitmapType*>(reinterpret_cast<char*>(m_Heap.Ptr) + m_Heap.Size);
	}
};

/// LinearHeapPolicyImpl<StoragePolicy, A, BlkSz, BlkCnt, Align>
template<class StoragePolicy, class A, size_t BlkSz, size_t BlkCnt, size_t Align>
class Epic::detail::LinearHeapPolicyImpl
	: private StoragePolicy
{
	static_assert(std::is_default_constructible<A>::value, "The heap backing allocator must be default-constructible.");

public:
	using type = Epic::detail::LinearHeapPolicyImpl<StoragePolicy, A, BlkSz, BlkCnt, Align>;
	using AllocatorType = A;
	using StoragePolicyType = StoragePolicy;

public:
	static constexpr size_t Alignment = StoragePolicy::Alignment;
	static constexpr size_t MinAllocSize = 0;
	static constexpr size_t MaxAllocSize = BlkSz * BlkCnt;

private:
	A m_Allocator;

protected:
	LinearHeapPolicyImpl() noexcept(std::is_nothrow_default_constructible<A>::value)
		: m_Allocator{ }, StoragePolicyType{ }
	{ }

	LinearHeapPolicyImpl(const type&) = delete;

	template<typename = std::enable_if_t<std::is_move_constructible<A>::value>>
	LinearHeapPolicyImpl(type&& obj) noexcept(std::is_nothrow_move_constructible<A>::value)
		: m_Allocator{ std::move(obj.m_Allocator) }, StoragePolicyType{ std::move(obj) }
	{ }

	LinearHeapPolicyImpl& operator = (const type&) = delete;
	LinearHeapPolicyImpl& operator = (type&& obj) = delete;

	~LinearHeapPolicyImpl()
	{
		FreeHeap(m_Allocator);
	}

protected:
	constexpr void* GetBlockPointer(size_t block) const noexcept
	{
		return static_cast<void*>(reinterpret_cast<char*>(m_Heap.Ptr) + (BlkSz * block));
	}

	constexpr size_t GetBlock(void* ptr) const noexcept
	{
		return (reinterpret_cast<char*>(ptr) - reinterpret_cast<char*>(GetBlockPointer(0))) / BlkSz;
	}

	constexpr size_t BytesToBlockSize(size_t bytes) const noexcept
	{
		return (bytes + BlkSz - 1) / BlkSz;
	}

public:
	constexpr bool Owns(const Blk& blk) const noexcept
	{
		return m_Heap && (blk.Ptr >= m_Heap.Ptr && blk.Ptr < GetBlockPointer(BlkCnt));
	}

	Blk Allocate(size_t sz) noexcept
	{
		// Verify heap memory
		if (!m_Heap) AllocateHeap(m_Allocator);
		if (!m_Heap) return{ nullptr, 0 };

		// Find a region of free blocks large enough to hold this allocation
		auto pBitmap = GetBitmapPointer();

		size_t blocksReq = BytesToBlockSize(sz);
		size_t block = pBitmap->FindAvailable(blocksReq);

		if (block >= BlkCnt) return{ nullptr, 0 };

		// Allocate the blocks
		Blk blk{ GetBlockPointer(block), sz };
		pBitmap->Set(block, blocksReq);

std::cout << "Allocated " << blocksReq << " blocks into location " << block << ":" << std::endl;
pBitmap->Output();
std::cout << std::endl;

		return blk;
	}

	bool Reallocate(Blk& blk, size_t sz)
	{
		assert(Owns(blk) && "LinearHeapInternalStoragePolicy::Reallocate - Attempted to reallocate a block that was not allocated by this allocator");

		auto pBitmap = GetBitmapPointer();
		
		size_t curBlock = GetBlock(blk.Ptr);
		size_t curBlocksReq = BytesToBlockSize(blk.Size);
		size_t newBlocksReq = BytesToBlockSize(sz);

		// If the allocation doesn't need to be expanded or shrunk,
		// reallocating is easy
		if (curBlocksReq == newBlocksReq)
		{
std::cout << "Reallocated " << curBlock << " in place:" << std::endl;
pBitmap->Output();
std::cout << std::endl;

			blk.Size = sz;
			return true;
		}

		// If the allocation must be expanded, first try to do so
		// in-place.  Try normal reallocation if that fails.
		if (sz > blk.Size)
		{
			size_t addBlocks = newBlocksReq - curBlocksReq;
			if (pBitmap->HasAvailable(curBlock + curBlocksReq, addBlocks))
			{
				// This allocation can be expanded in-place
				blk.Size = sz;
				pBitmap->Set(curBlock + curBlocksReq, addBlocks);
std::cout << "Reallocated " << curBlocksReq << " blocks to " << newBlocksReq 
<< " blocks in place at location " << curBlock << ":" << std::endl;
pBitmap->Output();
std::cout << std::endl;
				return true;
			}

			bool result = detail::Reallocator<type>::apply(*this, blk, sz);
			if (result)
			{
std::cout << "Reallocated block " << curBlock << " from " << curBlocksReq << " blocks to " << newBlocksReq 
	<< " blocks at block " << GetBlock(blk.Ptr) << ":" << std::endl;
pBitmap->Output();
std::cout << std::endl;
			}

			return result;
		}

		// The allocation must be shrunk
		size_t remBlocks = curBlocksReq - newBlocksReq;
		blk.Size = sz;
		pBitmap->Unset(curBlock + newBlocksReq, remBlocks);

std::cout << "Reallocated (shrunk) block " << curBlock << " down to " << newBlocksReq << " blocks:" << std::endl;
pBitmap->Output();
std::cout << std::endl;

		return true;
	}

public:
	void Deallocate(const Blk& blk)
	{
		if (!blk) return;
		assert(Owns(blk) && "LinearHeapInternalStoragePolicy::Deallocate - Attempted to free a block that was not allocated by this allocator");

		if (!m_Heap) return;

		auto pBitmap = GetBitmapPointer();
		size_t block = GetBlock(blk.Ptr);
		size_t blocksReq = BytesToBlockSize(blk.Size);

		pBitmap->Unset(block, blocksReq);

std::cout << "Deallocated " << blocksReq << " blocks from location " << block << ":" << std::endl;
pBitmap->Output();
std::cout << std::endl;
	}

	void DeallocateAll() noexcept
	{
		if (!m_Heap) return;

		auto pBitmap = GetBitmapPointer();
		size_t bitmapBlocks = BytesToBlockSize(BitmapSize);

		pBitmap->Unset(bitmapBlocks, BlkCnt - bitmapBlocks);

std::cout << "Deallocated (all) " << (BlkCnt - bitmapBlocks) << " blocks" << std::endl;
pBitmap->Output();
std::cout << std::endl;
	}
};

/// InternalLinearHeapPolicy<A, BlkSz, BlkCnt, Align>
template<class A, size_t BlkSz, size_t BlkCnt, size_t Align>
class Epic::detail::InternalLinearHeapPolicy
	: public Epic::detail::LinearHeapPolicyImpl<Epic::detail::LinearHeapInternalStoragePolicy<A, BlkSz, BlkCnt, Align>, A, BlkSz, BlkCnt, Align>
{
public:
	using type = Epic::detail::InternalLinearHeapPolicy<A, BlkSz, BlkCnt, Align>;

private:
	using StoragePolicyType = Epic::detail::LinearHeapInternalStoragePolicy<A, BlkSz, BlkCnt, Align>;
	using base = Epic::detail::LinearHeapPolicyImpl<StoragePolicyType, A, BlkSz, BlkCnt, Align>;

public:
	constexpr InternalLinearHeapPolicy() noexcept = default;

	InternalLinearHeapPolicy(const type&) = delete;
	constexpr InternalLinearHeapPolicy(type&& obj) = default;

	InternalLinearHeapPolicy& operator = (const type&) = delete;
	InternalLinearHeapPolicy& operator = (type&& obj) = delete;
};

/// ExternalLinearHeapPolicy<A, BlkSz, BlkCnt, Align>
template<class A, size_t BlkSz, size_t BlkCnt, size_t Align>
class Epic::detail::ExternalLinearHeapPolicy
	: public Epic::detail::LinearHeapPolicyImpl<Epic::detail::LinearHeapExternalStoragePolicy<A, BlkSz, BlkCnt, Align>, A, BlkSz, BlkCnt, Align>
{
public:
	using type = Epic::detail::ExternalLinearHeapPolicy<A, BlkSz, BlkCnt, Align>;

private:
	using StoragePolicyType = Epic::detail::LinearHeapExternalStoragePolicy<A, BlkSz, BlkCnt, Align>;
	using base = Epic::detail::LinearHeapPolicyImpl<StoragePolicyType, A, BlkSz, BlkCnt, Align>;

public:
	constexpr ExternalLinearHeapPolicy() noexcept = default;

	ExternalLinearHeapPolicy(const type&) = delete;
	constexpr ExternalLinearHeapPolicy(type&& obj) = default;

	ExternalLinearHeapPolicy& operator = (const type&) = delete;
	ExternalLinearHeapPolicy& operator = (type&& obj) = delete;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<size_t BlockSize, size_t BlockCount, class Allocator, size_t Alignment = 0>
	using HeapAllocator = detail::HeapAllocatorImpl<Allocator, detail::InternalLinearHeapPolicy, BlockSize, BlockCount, Alignment>;

	// TODO: Rename
	template<size_t BlockSize, size_t BlockCount, class Allocator, size_t Alignment = 0>
	using HeapStrictAllocator = detail::HeapAllocatorImpl<Allocator, detail::ExternalLinearHeapPolicy, BlockSize, BlockCount, Alignment>;

	template<size_t BlockSize, size_t BlockCount, class Allocator, size_t Alignment = 0>
	using StaticHeapAllocator = detail::HeapAllocatorImpl<Allocator, detail::StaticHeapPolicy, BlockSize, BlockCount, Alignment>;
}
