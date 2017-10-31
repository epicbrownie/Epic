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
	using Type = Epic::SegregatorAllocator<T, S, L>;
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
	static constexpr bool IsShareable = S::IsShareable && L::IsShareable;


	static_assert(MinAllocSize < Threshold, "Threshold must be greater than the small allocator's minimum allocation size.");
	static_assert(MaxAllocSize >= Threshold, "Threshold must be less than or equal to the large allocator's maximum allocation size.");

public:
	constexpr SegregatorAllocator()
		noexcept(std::is_nothrow_default_constructible<S>::value && std::is_nothrow_default_constructible<L>::value) = default;

	template<typename = std::enable_if_t<std::conjunction_v<std::is_copy_constructible<S>, std::is_copy_constructible<L>>>>
	constexpr SegregatorAllocator(const Type& obj)
		noexcept(std::is_nothrow_copy_constructible<S>::value && std::is_nothrow_copy_constructible<L>::value)
		: m_SAllocator{ obj.m_SAllocator }, L{ obj.m_LAllocator }
	{ }

	template<typename = std::enable_if_t<std::conjunction_v<std::is_move_constructible<S>, std::is_move_constructible<L>>>>
	constexpr SegregatorAllocator(Type&& obj)
		noexcept(std::is_nothrow_move_constructible<S>::value && std::is_nothrow_move_constructible<L>::value)
		: m_SAllocator{ std::move(obj.m_SAllocator) }, m_LAllocator{ std::move(obj.m_LAllocator) }
	{ }

	template<typename = std::enable_if_t<std::conjunction_v<std::is_copy_assignable<S>, std::is_copy_assignable<L>>>>
	SegregatorAllocator& operator = (const Type& obj)
		noexcept(std::is_nothrow_copy_assignable<S>::value && std::is_nothrow_copy_assignable<L>::value)
	{
		m_SAllocator = obj.m_SAllocator;
		m_LAllocator = obj.m_LAllocator;

		return *this;
	}

	template<typename = std::enable_if_t<std::conjunction_v<std::is_move_assignable<S>, std::is_move_assignable<L>>>>
	SegregatorAllocator& operator = (Type&& obj)
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
	template<typename = std::enable_if_t<std::disjunction_v<detail::CanAllocate<S>, detail::CanAllocate<L>>>>
	Blk Allocate(size_t sz) noexcept
	{
		if constexpr (detail::CanAllocate<S>::value && detail::CanAllocate<L>::value)
		{
			return (sz < T)
				? m_SAllocator.Allocate(sz)
				: m_LAllocator.Allocate(sz);
		}

		else if constexpr (detail::CanAllocate<S>::value)
		{
			return (sz < T) ? m_SAllocator.Allocate(sz) : Blk{};
		}

		else /* if constexpr (detail::CanAllocate<L>::value) */
		{
			return (sz < T) ? Blk{} : m_LAllocator.Allocate(sz);
		}
	}

	/* Returns a block of uninitialized memory (aligned to alignment).
	   The small allocator is used if sz is less than the threshold.
	   Otherwise, the large allocator is used. */
	template<typename = std::enable_if_t<std::disjunction_v<detail::CanAllocateAligned<S>, detail::CanAllocateAligned<L>>>>
	Blk AllocateAligned(size_t sz, size_t alignment = 0) noexcept
	{
		if constexpr (detail::CanAllocateAligned<S>::value && detail::CanAllocateAligned<L>::value)
		{
			return (sz < T) 
				? m_SAllocator.AllocateAligned(sz, (alignment == 0) ? S::Alignment : alignment)
				: m_LAllocator.AllocateAligned(sz, (alignment == 0) ? L::Alignment : alignment);
		}

		else if constexpr (detail::CanAllocateAligned<S>::value)
		{
			return (sz < T)
				? m_SAllocator.AllocateAligned(sz, (alignment == 0) ? S::Alignment : alignment)
				: Blk{};
		}

		else /* if constexpr (detail::CanAllocateAligned<L>::value) */
		{
			return (sz < T)
				? Blk{}
				: m_LAllocator.AllocateAligned(sz, (alignment == 0) ? L::Alignment : alignment);
		}
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
				if constexpr (detail::CanReallocate<S>::value)
					return m_SAllocator.Reallocate(blk, sz);
				else
					return false;
			}
			else
			{
				// Old Block: L, New Block: S
				if constexpr (detail::CanAllocate<S>::value)
				{
					auto newblk = m_SAllocator.Allocate(sz);
					if (!newblk.Ptr) return false;

					if (blk)
					{
						std::memcpy(newblk.Ptr, blk.Ptr, sz);

						if constexpr (detail::CanDeallocate<L>::value)
							m_LAllocator.Deallocate(blk);
					}

					blk = newblk;
					return true;
				}
				else
					return false;
			}
		}
		else
		{
			if (blk.Size >= T)
			{
				// Old Block: L, New Block: L
				if constexpr (detail::CanReallocate<L>::value)
					return m_LAllocator.Reallocate(blk, sz);
				else
					return false;
			}
			else
			{
				// Old Block: S, New Block: L
				if constexpr (detail::CanAllocate<L>::value)
				{
					auto newblk = m_LAllocator.Allocate(sz);
					if (!newblk.Ptr) return false;

					if (blk)
					{
						std::memcpy(newblk.Ptr, blk.Ptr, blk.Size);
						
						if constexpr (detail::CanDeallocate<S>::value)
							m_SAllocator.Deallocate(blk);
					}

					blk = newblk;
					return true;
				}
				else
					return false;
			}
		}
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
				if constexpr (detail::CanReallocateAligned<S>::value)
					return m_SAllocator.ReallocateAligned(blk, sz, (alignment == 0) ? S::Alignment : alignment);
				else
					return false;
			}
			else
			{
				// Old Block: L, New Block: S
				if constexpr (detail::CanAllocateAligned<S>::value)
				{
					auto newblk = m_SAllocator.AllocateAligned(sz, (alignment == 0) ? S::Alignment : alignment);
					if (!newblk.Ptr) return false;

					if (blk)
					{
						std::memcpy(newblk.Ptr, blk.Ptr, sz);

						if constexpr (detail::CanDeallocateAligned<L>::value)
							m_LAllocator.DeallocateAligned(blk);
					}

					blk = newblk;
					return true;
				}
				else
					return false;
			}
		}
		else
		{
			if (blk.Size >= T)
			{
				// Old Block: L, New Block: L
				if constexpr (detail::CanReallocateAligned<L>::value)
					return m_LAllocator.ReallocateAligned(blk, sz, (alignment == 0) ? L::Alignment : alignment);
				else
					return false;
			}
			else
			{
				// Old Block: S, New Block: L
				if constexpr (detail::CanAllocateAligned<L>::value)
				{
					auto newblk = m_LAllocator.AllocateAligned(sz, (alignment == 0) ? L::Alignment : alignment);
					if (!newblk.Ptr) return false;

					if (blk)
					{
						std::memcpy(newblk.Ptr, blk.Ptr, sz);

						if constexpr (detail::CanDeallocateAligned<S>::value)
							m_SAllocator.DeallocateAligned(blk);
					}

					blk = newblk;
					return true;
				}
				else
					return false;
			}
		}

		return true;
	}

public:
	/* Frees the memory for blk. */
	template<typename = std::enable_if_t<std::disjunction_v<detail::CanDeallocate<S>, detail::CanDeallocate<L>>>>
	void Deallocate(const Blk& blk)
	{
		if (blk.Size < T)
		{
			if constexpr (detail::CanDeallocate<S>::value)
				m_SAllocator.Deallocate(blk);
		}
		else
		{
			if constexpr (detail::CanDeallocate<L>::value)
				m_LAllocator.Deallocate(blk);
		}
	}

	/* Frees the memory for blk (blk needs to have been allocated with AllocateAligned) */
	template<typename = std::enable_if_t<std::disjunction_v<detail::CanDeallocateAligned<S>, detail::CanDeallocateAligned<L>>>>
	void DeallocateAligned(const Blk& blk)
	{
		if (blk.Size < T)
		{
			if constexpr (detail::CanDeallocateAligned<S>::value)
				m_SAllocator.DeallocateAligned(blk);
		}
		else
		{
			if constexpr (detail::CanDeallocateAligned<L>::value)
				m_LAllocator.DeallocateAligned(blk);
		}
	}

	/* Frees all of the memory of both allocators. */
	template<typename = std::enable_if_t<std::conjunction_v<detail::CanDeallocateAll<S>, detail::CanDeallocateAll<L>>>>
	void DeallocateAll() noexcept
	{
		m_SAllocator.DeallocateAll();
		m_LAllocator.DeallocateAll();
	}

public:
	/* Frees all of the memory in the small allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<S>::value>>
	void DeallocateAllSmall() noexcept
	{
		m_SAllocator.DeallocateAll();
	}

	/* Frees all of the memory in the large allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<L>::value>>
	void DeallocateAllLarge() noexcept
	{
		m_LAllocator.DeallocateAll();
	}
};
