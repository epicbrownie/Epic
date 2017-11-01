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
	template<class PrimaryAllocator, class FallbackAllocator>
	class FallbackAllocator;
}

//////////////////////////////////////////////////////////////////////////////

/// FallbackAllocator<P, F>
template<class P, class F>
class Epic::FallbackAllocator
{
	static_assert(std::is_default_constructible<P>::value, "The primary allocator must be default-constructible.");
	static_assert(std::is_default_constructible<F>::value, "The fallback allocator must be default-constructible.");

public:
	using Type = Epic::FallbackAllocator<P, F>;
	using PrimaryAllocatorType = P;
	using FallbackAllocatorType = F;
	
public:
	static constexpr size_t Alignment{ std::min(P::Alignment, F::Alignment) };
	static constexpr size_t MinAllocSize = std::min(P::MinAllocSize, F::MinAllocSize);
	static constexpr size_t MaxAllocSize = std::max(P::MaxAllocSize, F::MaxAllocSize);
	static constexpr bool IsShareable = P::IsShareable && F::IsShareable;

private:
	PrimaryAllocatorType m_PAllocator;
	FallbackAllocatorType m_FAllocator;

public:
	constexpr FallbackAllocator()
		noexcept(std::is_nothrow_default_constructible<P>::value && std::is_nothrow_default_constructible<F>::value) = default;

	template<typename = std::enable_if_t<std::conjunction_v<std::is_copy_constructible<P>, std::is_copy_constructible<F>>>>
	constexpr FallbackAllocator(const Type& obj)
		noexcept(std::is_nothrow_copy_constructible<P>::value && std::is_nothrow_copy_constructible<F>::value)
		: m_PAllocator{ obj.m_PAllocator }, m_FAllocator{ obj.m_FAllocator }
	{ }

	template<typename = std::enable_if_t<std::conjunction_v<std::is_move_constructible<P>, std::is_move_constructible<F>>>>
	constexpr FallbackAllocator(Type&& obj)
		noexcept(std::is_nothrow_move_constructible<P>::value && std::is_nothrow_move_constructible<F>::value)
		: m_PAllocator{ std::move(obj.m_PAllocator) }, m_FAllocator{ std::move(obj.m_FAllocator) }
	{ }

	template<typename = std::enable_if_t<std::conjunction_v<std::is_copy_assignable<P>, std::is_copy_assignable<F>>>>
	FallbackAllocator& operator = (const Type& obj)
		noexcept(std::is_nothrow_copy_assignable<P>::value && std::is_nothrow_copy_assignable<F>::value)
	{
		m_PAllocator = obj.m_PAllocator;
		m_FAllocator = obj.m_FAllocator;

		return *this;
	}

	template<typename = std::enable_if_t<std::conjunction_v<std::is_move_assignable<P>, std::is_move_assignable<F>>>>
	FallbackAllocator& operator = (Type&& obj)
		noexcept(std::is_nothrow_move_assignable<P>::value && std::is_nothrow_move_assignable<F>::value)
	{
		m_PAllocator = std::move(obj.m_PAllocator);
		m_FAllocator = std::move(obj.m_FAllocator);

		return *this;
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	inline bool Owns(const Blk& blk) const noexcept
	{
		return m_PAllocator.Owns(blk) || m_FAllocator.Owns(blk);
	}

public:
	/* Returns a block of uninitialized memory.
	   Attempts to allocate using the Primary allocator.  The Fallback allocator is used 
	   if the Primary allocator returns a null pointer. */
	template<typename = std::enable_if_t<std::disjunction_v<detail::CanAllocate<P>, detail::CanAllocate<F>>>>
	Blk Allocate(size_t sz) noexcept
	{
		if constexpr (detail::CanAllocate<P>::value)
		{
			if (Blk result = m_PAllocator.Allocate(sz); result)
				return result;
		}

		if constexpr (detail::CanAllocate<F>::value)
			return m_FAllocator.Allocate(sz);
	}

	/* Returns a block of uninitialized memory (aligned to alignment).
	   Attempts to allocate using the Primary allocator.  The Fallback allocator is used 
	   if the Primary allocator returns a null pointer. */
	template<typename = std::enable_if_t<std::conjunction_v<detail::CanAllocateAligned<P>, detail::CanAllocateAligned<F>>>>
	Blk AllocateAligned(size_t sz, size_t alignment = 0) noexcept
	{
		Blk result = m_PAllocator.AllocateAligned(sz, (alignment == 0) ? P::Alignment : alignment);

		if (!result)
			result = m_FAllocator.AllocateAligned(sz, (alignment == 0) ? F::Alignment : alignment);

		return result;
	}

	/* Attempts to reallocate the memory of blk to the new size sz. */
	template<typename = std::enable_if_t<std::disjunction_v<detail::CanReallocate<P>, detail::CanReallocate<F>>>>
	bool Reallocate(Blk& blk, size_t sz)
	{
		if (m_PAllocator.Owns(blk))
		{
			if constexpr (detail::CanReallocate<P>::value)
				return m_PAllocator.Reallocate(blk, sz);
			else
				return false;
		}
		else
		{
			if constexpr (detail::CanReallocate<F>::value)
				return m_FAllocator.Reallocate(blk, sz);
			else
				return false;
		}
	}

	/* Attempts to reallocate the memory of blk (aligned to alignment) to the new size sz. */
	template<typename = std::enable_if_t<std::disjunction_v<detail::CanReallocateAligned<P>, detail::CanReallocateAligned<F>>>>
	bool ReallocateAligned(Blk& blk, size_t sz, size_t alignment = 0)
	{
		if (m_PAllocator.Owns(blk))
		{
			if constexpr (detail::CanReallocateAligned<P>::value)
				return m_PAllocator.ReallocateAligned(blk, sz, (alignment == 0) ? P::Alignment : alignment);
			else
				return false;
		}
		else
		{
			if constexpr (detail::CanReallocateAligned<F>::value)
				return m_FAllocator.ReallocateAligned(blk, sz, (alignment == 0) ? F::Alignment : alignment);
			else 
				return false;
		}
	}

public:
	/* Frees the memory for blk. */
	template<typename = std::enable_if_t<std::disjunction_v<detail::CanDeallocate<P>, detail::CanDeallocate<F>>>>
	void Deallocate(const Blk& blk)
	{
		if (m_PAllocator.Owns(blk))
		{
			if constexpr (detail::CanDeallocate<P>::value)
				m_PAllocator.Deallocate(blk);
		}
		else
		{
			assert(m_FAllocator.Owns(blk) && 
				"FallbackAllocator::Deallocate() - "
				"Attempted to free a block that was not allocated by this allocator");

			if constexpr (detail::CanDeallocate<F>::value)
				m_FAllocator.Deallocate(blk);
		}
	}

	/* Frees the memory for blk (blk needs to have been allocated with AllocateAligned). */
	template<typename = std::enable_if_t<std::conjunction_v<
		detail::CanAllocateAligned<P>, detail::CanAllocateAligned<F>, 
		std::disjunction<detail::CanDeallocateAligned<P>, detail::CanDeallocateAligned<F>>>>
	void DeallocateAligned(const Blk& blk)
	{
		if (m_PAllocator.Owns(blk))
		{
			if constexpr (detail::CanDeallocateAligned<P>::value)
				m_PAllocator.DeallocateAligned(blk);
		}
		else
		{
			assert(m_FAllocator.Owns(blk) && 
				"FallbackAllocator::DeallocateAligned() - "
				"Attempted to free a block that was not allocated by this allocator");

			if constexpr (detail::CanDeallocateAligned<F>::value)
				m_FAllocator.DeallocateAligned(blk);
		}
	}

	/* Frees all of the memory in both allocators. */
	template<typename = std::enable_if_t<std::conjunction_v<detail::CanDeallocateAll<P>, detail::CanDeallocateAll<F>>>>
	void DeallocateAll() noexcept
	{
		m_PAllocator.DeallocateAll();
		m_FAllocator.DeallocateAll();
	}

public:
	/* Frees all of the memory in the primary allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<P>::value>>
	void DeallocateAllPrimary() noexcept
	{
		m_PAllocator.DeallocateAll();
	}

	/* Frees all of the memory in the fallback allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<F>::value>>
	void DeallocateAllFallback() noexcept
	{
		m_FAllocator.DeallocateAll();
	}
};
