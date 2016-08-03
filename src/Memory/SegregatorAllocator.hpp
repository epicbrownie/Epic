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

#include <Epic/Memory/AllocatorTraits.hpp>
#include <Epic/Memory/AllocatorHelpers.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <algorithm>
#include <cstdint>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<size_t BytesThreshold, class SmallAllocator, class LargeAllocator>
	class SegregatorAllocator;
}

//////////////////////////////////////////////////////////////////////////////

/// SegregatorAllocator<T, S, L>
template<size_t T, class S, class L>
class Epic::SegregatorAllocator
{
	static_assert(std::is_default_constructible<S>::value, "The small allocator must be default-constructible.");
	static_assert(std::is_default_constructible<L>::value, "The large allocator must be default-constructible.");

public:
	using type = Epic::SegregatorAllocator<T, S, L>;
	using SmallAllocatorType = S;
	using LargeAllocatorType = L;

private:
	SmallAllocatorType m_SAllocator;
	LargeAllocatorType m_LAllocator;

public:
	static constexpr size_t Alignment{ std::min(S::Alignment, L::Alignment) };
	static constexpr size_t MinAllocSize = S::MinAllocSize;
	static constexpr size_t MaxAllocSize = L::MaxAllocSize;
	static constexpr size_t Threshold{ T };

	static_assert(MinAllocSize < Threshold, "Threshold must be greater than the small allocator's minimum allocation size.");
	static_assert(MaxAllocSize >= Threshold, "Threshold must be less than or equal to the large allocator's maximum allocation size.");

public:
	constexpr SegregatorAllocator()
		noexcept(std::is_nothrow_default_constructible<S>::value && std::is_nothrow_default_constructible<L>::value) = default;

	template<typename = std::enable_if_t<std::is_copy_constructible<S>::value && std::is_copy_constructible<L>::value>>
	constexpr SegregatorAllocator(const SegregatorAllocator<T, S, L>& obj)
		noexcept(std::is_nothrow_copy_constructible<S>::value && std::is_nothrow_copy_constructible<L>::value)
		: m_SAllocator{ obj.m_SAllocator }, L{ obj.m_LAllocator }
	{ }

	template<typename = std::enable_if_t<std::is_move_constructible<S>::value && std::is_move_constructible<L>::value>>
	constexpr SegregatorAllocator(SegregatorAllocator<T, S, L>&& obj)
		noexcept(std::is_nothrow_move_constructible<S>::value && std::is_nothrow_move_constructible<L>::value)
		: m_SAllocator{ std::move(obj.m_SAllocator) }, m_LAllocator{ std::move(obj.m_LAllocator) }
	{ }

	template<typename = std::enable_if_t<std::is_copy_assignable<S>::value && std::is_copy_assignable<L>::value>>
	SegregatorAllocator& operator = (const SegregatorAllocator<T, S, L>& obj)
		noexcept(std::is_nothrow_copy_assignable<S>::value && std::is_nothrow_copy_assignable<L>::value)
	{
		m_SAllocator = obj.m_SAllocator;
		m_LAllocator = obj.m_LAllocator;

		return *this;
	}

	template<typename = std::enable_if_t<std::is_move_assignable<S>::value && std::is_move_assignable<L>::value>>
	SegregatorAllocator& operator = (SegregatorAllocator<T, S, L>&& obj)
		noexcept(std::is_nothrow_move_assignable<S>::value && std::is_nothrow_move_assignable<L>::value)
	{
		m_SAllocator = std::move(obj.m_SAllocator);
		m_LAllocator = std::move(obj.m_LAllocator);

		return *this;
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	inline bool Owns(const Blk& blk) const noexcept
	{
		return (blk.Size < T) ? m_SAllocator.Owns(blk) : m_LAllocator.Owns(blk);
	}

public:
	/* Returns a block of uninitialized memory.
	   The small allocator is used if sz is less than the threshold.
	   Otherwise, the large allocator is used. */
	template<typename = std::enable_if_t<detail::CanAllocate<S>::value || detail::CanAllocate<L>::value>>
	Blk Allocate(size_t sz) noexcept
	{
		return (sz < T) ? 
			detail::AllocateIf<S>::apply(m_SAllocator, sz) : 
			detail::AllocateIf<L>::apply(m_LAllocator, sz);
	}

	/* Returns a block of uninitialized memory (aligned to alignment).
	   The small allocator is used if sz is less than the threshold.
	   Otherwise, the large allocator is used. */
	template<typename = std::enable_if_t<detail::CanAllocateAligned<S>::value || detail::CanAllocateAligned<L>::value>>
	Blk AllocateAligned(size_t sz, size_t alignment = 0) noexcept
	{
		if (sz < T)
			return detail::AllocateAlignedIf<S>::apply(m_SAllocator, sz, (alignment == 0) ? S::Alignment : alignment);
		else
			return detail::AllocateAlignedIf<L>::apply(m_LAllocator, sz, (alignment == 0) ? L::Alignment : alignment);
	}

	/* Attempts to reallocate the memory of blk to the new size sz.
	   This might result in a change of allocator for blk. */
	bool Reallocate(Blk& blk, size_t sz)
	{
		if (sz < T)
		{
			if (blk.Size < T)
			{
				// Old Block: S, New Block: S
				return detail::ReallocateIf<S>::apply(m_SAllocator, blk, sz);
			}
			else
			{
				// Old Block: L, New Block: S
				auto newblk = detail::AllocateIf<S>::apply(m_SAllocator, sz);
				if (!newblk.Ptr) return false;

				if (blk)
				{
					std::memcpy(newblk.Ptr, blk.Ptr, sz);
					detail::DeallocateIf<L>::apply(m_LAllocator, blk);
				}

				blk = newblk;
			}
		}
		else
		{
			if (blk.Size >= T)
			{
				// Old Block: L, New Block: L
				return detail::ReallocateIf<L>::apply(m_LAllocator, blk, sz);
			}
			else
			{
				// Old Block: S, New Block: L
				auto newblk = detail::AllocateIf<L>::apply(m_LAllocator, sz);
				if (!newblk.Ptr) return false;

				if (blk)
				{
					std::memcpy(newblk.Ptr, blk.Ptr, blk.Size);
					detail::DeallocateIf<S>::apply(m_SAllocator, blk);
				}

				blk = newblk;
			}
		}

		return true;
	}

	/* Attempts to reallocate the memory of blk (aligned to alignment) to the new size sz. 
	   This might result in a change of allocator for blk. */
	bool ReallocateAligned(Blk& blk, size_t sz, size_t alignment = 0)
	{
		if (sz < T)
		{
			if (blk.Size < T)
			{
				// Old Block: S, New Block: S
				return detail::ReallocateAlignedIf<S>::apply(m_SAllocator, blk, sz, (alignment == 0) ? S::Alignment : alignment);
			}
			else
			{
				// Old Block: L, New Block: S
				auto newblk = detail::AllocateAlignedIf<S>::apply(m_SAllocator, sz, (alignment == 0) ? S::Alignment : alignment);
				if (!newblk.Ptr) return false;

				if (blk)
				{
					std::memcpy(newblk.Ptr, blk.Ptr, sz);
					detail::DeallocateAlignedIf<L>::apply(m_LAllocator, blk);
				}

				blk = newblk;
			}
		}
		else
		{
			if (blk.Size >= T)
			{
				// Old Block: L, New Block: L
				return detail::ReallocateAlignedIf<L>::apply(m_LAllocator, blk, sz, (alignment == 0) ? L::Alignment : alignment);
			}
			else
			{
				// Old Block: S, New Block: L
				auto newblk = detail::AllocateAlignedIf<L>::apply(m_LAllocator, sz, (alignment == 0) ? L::Alignment : alignment);
				if (!newblk.Ptr) return false;

				if (blk)
				{
					std::memcpy(newblk.Ptr, blk.Ptr, blk.Size);
					detail::DeallocateAlignedIf<S>::apply(m_SAllocator, blk);
				}

				blk = newblk;
			}
		}

		return true;
	}

public:
	/* Frees the memory for blk. */
	template<typename = std::enable_if_t<detail::CanDeallocate<S>::value || detail::CanDeallocate<L>::value>>
	void Deallocate(const Blk& blk)
	{
		if (blk.Size < T)
			detail::DeallocateIf<S>::apply(m_SAllocator, blk);
		else
			detail::DeallocateIf<L>::apply(m_LAllocator, blk);
	}

	/* Frees the memory for blk (blk needs to have been allocated with AllocateAligned) */
	template<typename = std::enable_if_t<detail::CanDeallocateAligned<S>::value || detail::CanDeallocateAligned<L>::value>>
	void DeallocateAligned(const Blk& blk)
	{
		if (blk.Size < T)
			detail::DeallocateAlignedIf<S>::apply(m_SAllocator, blk);
		else
			detail::DeallocateAlignedIf<L>::apply(m_LAllocator, blk);
	}

	/* Frees all of the memory of both allocators. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<S>::value && detail::CanDeallocateAll<L>::value>>
	void DeallocateAll()
	{
		m_SAllocator.DeallocateAll();
		m_LAllocator.DeallocateAll();
	}

public:
	/* Frees all of the memory in the small allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<S>::value>>
	void DeallocateAllSmall()
	{
		m_SAllocator.DeallocateAll();
	}

	/* Frees all of the memory in the large allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<L>::value>>
	void DeallocateAllLarge()
	{
		m_LAllocator.DeallocateAll();
	}

private:
	void* operator new (size_t) noexcept = delete;
	void* operator new[] (size_t) noexcept = delete;
	void operator delete (void*) noexcept = delete;
	void operator delete[] (void*) noexcept = delete;
};
