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
#include <Epic/Memory/detail/ForceAlignHelpers.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class Allocator, size_t ForcedAlignment = 0>
	class ForceAlignAllocator;
}

//////////////////////////////////////////////////////////////////////////////

/// ForceAlignAllocator<A, ForcedAlignment>
template<class A, size_t ForcedAlignment>
class Epic::ForceAlignAllocator
{
	static_assert(std::is_default_constructible<A>::value, "The allocator must be default-constructible.");
	static_assert(detail::CanAllocate<A>::value || detail::CanAllocateAligned<A>::value, 
		"The allocator must support allocations.");

public:
	using Type = Epic::ForceAlignAllocator<A, ForcedAlignment>;
	using AllocatorType = A;
	
public:
	static constexpr size_t Alignment = ForcedAlignment;
	static constexpr size_t MinAllocSize = A::MinAllocSize;
	static constexpr size_t MaxAllocSize = A::MaxAllocSize;

	static_assert(detail::IsGoodAlignment(Alignment), "ForceAlignAllocator still requires a valid alignment value.");

private:
	AllocatorType m_Allocator;

public:
	constexpr ForceAlignAllocator() 
		noexcept(std::is_nothrow_default_constructible<A>::value) = default;

	template<typename = std::enable_if_t<std::is_copy_constructible<A>::value>>
	constexpr ForceAlignAllocator(const Type& obj)
		noexcept(std::is_nothrow_copy_constructible<A>::value)
		: m_Allocator{ obj.m_Allocator }
	{ }

	template<typename = std::enable_if_t<std::is_move_constructible<A>::value>>
	constexpr ForceAlignAllocator(Type&& obj)
		noexcept(std::is_nothrow_move_constructible<A>::value)
		: m_Allocator{ std::move(obj.m_Allocator) }
	{ }

	template<typename = std::enable_if_t<std::is_copy_assignable<A>::value>>
	ForceAlignAllocator& operator = (const Type& obj)
		noexcept(std::is_nothrow_copy_assignable<A>::value)
	{
		m_Allocator = obj.m_Allocator;
	
		return *this;
	}

	template<typename = std::enable_if_t<std::is_move_assignable<A>::value>>
	ForceAlignAllocator& operator = (Type&& obj)
		noexcept(std::is_nothrow_move_assignable<A>::value)
	{
		m_Allocator = std::move(obj.m_Allocator);

		return *this;
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	inline bool Owns(const Blk& blk) const noexcept
	{
		return m_Allocator.Owns(blk);
	}

public:
	/* Returns a block of uninitialized memory (aligned to ForcedAlignment). */
	Blk Allocate(size_t sz) noexcept
	{
		// Verify that the requested size is within our allowed bounds
		if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
			return{ nullptr, 0 };

		// Allocate the block
		Blk blk;

		if (detail::CanAllocateAligned<A>::value)
		{
			blk = detail::AllocateAlignedIf<A>::apply(m_Allocator, sz, Alignment);
			if (blk) return blk;
		}

		// Allocating aligned failed. Force a normal allocation to be aligned.
		size_t blockSpace = sz + Alignment - 1;
		size_t newsz = blockSpace + sizeof(detail::ForceAlignSuffix);
		
		blk = detail::AllocateIf<A>::apply(m_Allocator, newsz);
		if (!blk) return{ nullptr, 0 };
		
		// Calculate an aligned region
		size_t space = blockSpace;
		void* pAlignedRegion = blk.Ptr;

		pAlignedRegion = std::align(Alignment, sz, pAlignedRegion, space);
		assert(pAlignedRegion != nullptr); // Alignment should never fail here

		// Store alignment padding value in our suffix
		detail::ForceAlignSuffix::Set(blk, blockSpace - space);
		
		return{ pAlignedRegion, space };
	}

	/* Returns a block of uninitialized memory (aligned to alignment).
	   ForcedAlignment will not be enforced. */
	template<typename = std::enable_if_t<detail::CanAllocateAligned<A>::value>>
	Blk AllocateAligned(size_t sz, size_t alignment = Alignment) noexcept
	{
		return m_Allocator.AllocateAligned(sz, alignment);
	}

	/* Attempts to reallocate the memory of blk to the new size sz (aligned to ForcedAlignment). */
	bool Reallocate(Blk& blk, size_t sz)
	{
		// If the block isn't valid, delegate to Allocate
		if (!blk)
		{
			blk = detail::AllocateIf<Type>::apply(*this, sz);
			return (bool)blk;
		}

		// If the requested size is zero, delegate to Deallocate
		if (sz == 0)
		{
			if (detail::CanDeallocate<Type>::value)
			{
				detail::DeallocateIf<Type>::apply(*this, blk);
				blk = { nullptr, 0 };
			}

			return detail::CanDeallocate<Type>::value;
		}

		// Verify that the requested size is within our allowed bounds
		if (sz < MinAllocSize || sz > MaxAllocSize)
			return false;

		return detail::Reallocator<Type>::ReallocateViaCopy(*this, blk, sz);
	}

	/* Attempts to reallocate the memory of blk (aligned to alignment) to the new size sz. 
	   ForcedAlignment will not be enforced. */
	template<typename = std::enable_if_t<detail::CanReallocateAligned<A>::value>>
	bool ReallocateAligned(Blk& blk, size_t sz, size_t alignment = Alignment)
	{
		return m_Allocator.ReallocateAligned(blk, sz, alignment);
	}

	/* Returns a block of uninitialized memory equal to the total remaining amount
	   of available memory (aligned to ForcedAlignment). */
	template<typename = std::enable_if_t<detail::CanAllocateAll<A>::value>>
	Blk AllocateAll() noexcept
	{
		// Allocate the block
		Blk blk;

		if (detail::CanAllocateAllAligned<A>::value)
		{
			blk = detail::AllocateAllAlignedIf<A>::apply(m_Allocator, Alignment);
			if (blk) return blk;
		}

		// Allocating aligned failed. Force a normal allocation to be aligned.
		blk = detail::AllocateAllIf<A>::apply(m_Allocator);
		if (!blk) return{ nullptr, 0 };

		// Make sure this block is large enough to store our prefix and a byte
		size_t spaceReq = 1 + sizeof(detail::ForceAlignSuffix);

		if (blk.Size < spaceReq)
		{
			detail::DeallocateIf<A>::apply(m_Allocator, blk);
			return{ nullptr,0 };
		}

		// Attempt to align the block
		size_t space = blk.Size - sizeof(detail::ForceAlignSuffix);
		void* pAlignedRegion = blk.Ptr;

		if (!std::align(Alignment, 1, pAlignedRegion, space))
		{
			detail::DeallocateIf<A>::apply(m_Allocator, blk);
			return{ nullptr, 0 };
		}
		
		// Store alignment padding value in our suffix
		detail::ForceAlignSuffix::Set(blk, blk.Size - sizeof(detail::ForceAlignSuffix) - space);

		return{ pAlignedRegion, space };
	}

	/* Returns a block of unitialized memory equal to the total remaining amount 
	   of available memory (aligned to alignment).
	   ForcedAlignment will not be enforced. */
	template<typename = std::enable_if_t<detail::CanAllocateAllAligned<A>::value>>
	Blk AllocateAllAligned(size_t alignment = Alignment) noexcept
	{
		return m_Allocator.AllocateAllAligned(alignment);
	}

public:
	/* Frees the memory for blk. */
	void Deallocate(const Blk& blk)
	{
		if (!blk) return;

		assert(Owns(blk) && "ForceAlignAllocator::Deallocate - Attempted to free a block that was not allocated by this allocator");

		if (detail::CanAllocateAligned<A>::value)
		{
			// AllocateAligned() was used
			detail::DeallocateAlignedIf<A>::apply(m_Allocator, blk);
			return;
		}

		// Calculate the actual block
		size_t alignPad = detail::ForceAlignSuffix::Get(blk);

		Blk actualblk = 
		{
			static_cast<void*>(reinterpret_cast<char*>(blk.Ptr) - alignPad),
			blk.Size + alignPad + sizeof(detail::ForceAlignSuffix)
		};

		// Free the block
		detail::DeallocateIf<A>::apply(m_Allocator, actualblk);
	}

	/* Frees the memory for blk. This should only be called if AllocateAligned() 
	   or AllocateAllAligned() was used to allocate blk. */
	template<typename = std::enable_if_t<detail::CanDeallocateAligned<A>::value>>
	void DeallocateAligned(const Blk& blk)
	{
		m_Allocator.DeallocateAligned(blk);
	}

	/* Frees all of the memory of the allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<A>::value>>
	void DeallocateAll() noexcept
	{
		m_Allocator.DeallocateAll();
	}
};
