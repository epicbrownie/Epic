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

#include <Epic/Memory/detail/AllocatorTraits.hpp>
#include <Epic/Memory/detail/AllocatorHelpers.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <cstdint>
#include <algorithm>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class AlignedAllocator, class UnalignedAllocator>
	class AlignmentAllocator;
}

//////////////////////////////////////////////////////////////////////////////

/// AlignmentAllocator<A, U>
template<class A, class U>
class Epic::AlignmentAllocator
{
	static_assert(std::is_default_constructible<A>::value, "The aligned allocator must be default-constructible.");
	static_assert(std::is_default_constructible<U>::value, "The unaligned allocator must be default-constructible.");
	static_assert(detail::CanAllocateAligned<A>::value, "The aligned allocator must support aligned allocations.");
	static_assert(detail::CanAllocate<U>::value, "The unaligned allocator must support unaligned allocations.");

public:
	using type = Epic::AlignmentAllocator<A, U>;
	using AlignedAllocatorType = A;
	using UnalignedAllocatorType = U;

public:
	static constexpr size_t Alignment{ std::min(A::Alignment, U::Alignment) };
	static constexpr size_t MinAllocSize = std::min(A::MinAllocSize, U::MinAllocSize);
	static constexpr size_t MaxAllocSize = std::max(A::MaxAllocSize, U::MaxAllocSize);

private:
	AlignedAllocatorType m_AAllocator;
	UnalignedAllocatorType m_UAllocator;

public:
	constexpr AlignmentAllocator()
		noexcept(std::is_nothrow_default_constructible<A>::value && std::is_nothrow_default_constructible<U>::value) = default;

	template<typename = std::enable_if_t<std::is_copy_constructible<A>::value && std::is_copy_constructible<U>::value>>
	constexpr AlignmentAllocator(const AlignmentAllocator<A, U>& obj)
		noexcept(std::is_nothrow_copy_constructible<A>::value && std::is_nothrow_copy_constructible<U>::value)
		: m_AAllocator{ obj.m_AAllocator }, m_UAllocator{ obj.m_UAllocator }
	{ }

	template<typename = std::enable_if_t<std::is_move_constructible<A>::value && std::is_move_constructible<U>::value>>
	constexpr AlignmentAllocator(AlignmentAllocator<A, U>&& obj)
		noexcept(std::is_nothrow_move_constructible<A>::value && std::is_nothrow_move_constructible<U>::value)
		: m_AAllocator{ std::move(obj.m_AAllocator) }, m_UAllocator{ std::move(obj.m_UAllocator) }
	{ }

	template<typename = std::enable_if_t<std::is_copy_assignable<A>::value && std::is_copy_assignable<U>::value>>
	AlignmentAllocator& operator = (const AlignmentAllocator<A, U>& obj)
		noexcept(std::is_nothrow_copy_assignable<A>::value && std::is_nothrow_copy_assignable<U>::value)
	{
		m_AAllocator = obj.m_AAllocator;
		m_UAllocator = obj.m_UAllocator;
		
		return *this;
	}

	template<typename = std::enable_if_t<std::is_move_assignable<A>::value && std::is_move_assignable<U>::value>>
	AlignmentAllocator& operator = (AlignmentAllocator<A, U>&& obj)
		noexcept(std::is_nothrow_move_assignable<A>::value && std::is_nothrow_move_assignable<U>::value)
	{
		m_AAllocator = std::move(obj.m_AAllocator);
		m_UAllocator = std::move(obj.m_UAllocator);

		return *this;
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	inline bool Owns(const Blk& blk) const noexcept
	{
		return m_AAllocator.Owns(blk) || m_UAllocator.Owns(blk);
	}

public:
	/* Returns a block of uninitialized memory.
	   Uses the unaligned allocator. */
	Blk Allocate(size_t sz) noexcept
	{
		return m_UAllocator.Allocate(sz);
	}

	/* Returns a block of uninitialized memory (aligned to alignment).
	   Uses the aligned allocator. */
	Blk AllocateAligned(size_t sz, size_t alignment = A::Alignment) noexcept
	{
		return m_AAllocator.AllocateAligned(sz, alignment);
	}

	/* Attempts to reallocate the memory of blk to the new size sz.
	   Uses the unaligned allocator. */
	template<typename = std::enable_if_t<detail::CanReallocate<U>::value>>
	bool Reallocate(Blk& blk, size_t sz)
	{
		return m_UAllocator.Reallocate(blk, sz);
	}

	/* Attempts to reallocate the memory of blk (aligned to alignment) to the new size sz.
	   Uses the aligned allocator. */
	template<typename = std::enable_if_t<detail::CanReallocateAligned<A>::value>>
	bool ReallocateAligned(Blk& blk, size_t sz, size_t alignment = A::Alignment)
	{
		return m_AAllocator.ReallocateAligned(blk, sz, alignment);
	}

	/* Returns a block of uninitialized memory.
	   Its size is all of the remaining memory in the unaligned allocator. */
	template<typename = std::enable_if_t<detail::CanAllocateAll<U>::value>>
	Blk AllocateAll() noexcept
	{
		return m_UAllocator.AllocateAll();
	}

	/* Returns a block of uninitialized memory.
	   Its size is all of the remaining memory in the aligned allocator (aligned to alignment). */
	template<typename = std::enable_if_t<detail::CanAllocateAll<A>::value>>
	Blk AllocateAllAligned(size_t alignment = A::Alignment) noexcept
	{
		return m_AAllocator.AllocateAllAligned(alignment);
	}

public:
	/* Frees the memory for blk.  Uses the unaligned allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocate<U>::value>>
	void Deallocate(const Blk& blk)
	{
		m_UAllocator.Deallocate(blk);
	}

	/* Frees the memory for blk.  Uses the aligned allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAligned<A>::value>>
	void DeallocateAligned(const Blk& blk)
	{
		m_AAllocator.DeallocateAligned(blk);
	}

	/* Frees all of the memory of both allocators. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<U>::value && detail::CanDeallocateAll<A>::value>>
	void DeallocateAll()
	{
		m_AAllocator.DeallocateAll();
		m_UAllocator.DeallocateAll();
	}

public:
	/* Frees all of the memory of the aligned allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<A>::value>>
	void DeallocateAllAligned()
	{
		m_AAllocator.DeallocateAll();
	}

	/* Frees all of the memory of the unaligned allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<U>::value>>
	void DeallocateAllUnaligned()
	{
		m_UAllocator.DeallocateAll();
	}

private:
	void* operator new (size_t) noexcept = delete;
	void* operator new[] (size_t) noexcept = delete;
	void operator delete (void*) noexcept = delete;
	void operator delete[] (void*) noexcept = delete;
};
