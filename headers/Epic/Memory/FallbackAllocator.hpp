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
	template<class PrimaryAllocator, class FallbackAllocator>
	class FallbackAllocator;
}

//////////////////////////////////////////////////////////////////////////////

/// FallbackAllocator<P, F>
template<class P, class F>
class Epic::FallbackAllocator 
	: private P, private F
{
	static_assert(std::is_default_constructible<P>::value, "The primary allocator must be default-constructible.");
	static_assert(std::is_default_constructible<F>::value, "The fallback allocator must be default-constructible.");

public:
	using type = Epic::FallbackAllocator<P, F>;
	using PrimaryAllocatorType = P;
	using FallbackAllocatorType = F;
	
public:
	static constexpr size_t Alignment{ std::min(P::Alignment, F::Alignment) };
	static constexpr size_t MinAllocSize = std::min(P::MinAllocSize, F::MinAllocSize);
	static constexpr size_t MaxAllocSize = std::max(P::MaxAllocSize, F::MaxAllocSize);

public:
	constexpr FallbackAllocator()
		noexcept(std::is_nothrow_default_constructible<P>::value && std::is_nothrow_default_constructible<F>::value) = default;

	template<typename = std::enable_if_t<std::is_copy_constructible<P>::value && std::is_copy_constructible<F>::value>>
	constexpr FallbackAllocator(const FallbackAllocator<P, F>& obj)
		noexcept(std::is_nothrow_copy_constructible<P>::value && std::is_nothrow_copy_constructible<F>::value)
		: P{ obj }, F{ obj } 
	{ }

	template<typename = std::enable_if_t<std::is_move_constructible<P>::value && std::is_move_constructible<F>::value>>
	constexpr FallbackAllocator(FallbackAllocator<P, F>&& obj)
		noexcept(std::is_nothrow_move_constructible<P>::value && std::is_nothrow_move_constructible<F>::value)
		: P{ std::move(obj) }, F{ std::move(obj) }
	{ }

	template<typename = std::enable_if_t<std::is_copy_assignable<P>::value && std::is_copy_assignable<F>::value>>
	FallbackAllocator& operator = (const FallbackAllocator<P, F>& obj)
		noexcept(std::is_nothrow_copy_assignable<P>::value && std::is_nothrow_copy_assignable<F>::value)
	{
		P::operator = (obj);
		F::operator = (obj);

		return *this;
	}

	template<typename = std::enable_if_t<std::is_move_assignable<P>::value && std::is_move_assignable<F>::value>>
	FallbackAllocator& operator = (FallbackAllocator<P, F>&& obj)
		noexcept(std::is_nothrow_move_assignable<P>::value && std::is_nothrow_move_assignable<F>::value)
	{
		P::operator = (std::move(obj));
		F::operator = (std::move(obj));

		return *this;
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	inline bool Owns(const Blk& blk) const noexcept
	{
		return P::Owns(blk) || F::Owns(blk);
	}

public:
	/* Returns a block of uninitialized memory.
	   Attempts to allocate using the Primary allocator.  The Fallback allocator is used 
	   if the Primary allocator returns a null pointer. */
	template<typename = std::enable_if_t<detail::CanAllocate<P>::value || detail::CanAllocate<F>::value>>
	Blk Allocate(size_t sz) noexcept
	{
		Blk result = detail::AllocateIf<P>::apply(*this, sz);
		
		if (!result)
			result = detail::AllocateIf<F>::apply(*this, sz);

		return result;
	}

	/* Returns a block of uninitialized memory (aligned to alignment).
	   Attempts to allocate using the Primary allocator.  The Fallback allocator is used 
	   if the Primary allocator returns a null pointer. */
	template<typename = std::enable_if_t<detail::CanAllocateAligned<P>::value && detail::CanAllocateAligned<F>::value>>
	Blk AllocateAligned(size_t sz, size_t alignment = 0) noexcept
	{
		Blk result = P::AllocateAligned(sz, (alignment == 0) ? P::Alignment : alignment);

		if (!result)
			result = F::AllocateAligned(sz, (alignment == 0) ? F::Alignment : alignment);

		return result;
	}

	/* Attempts to reallocate the memory of blk to the new size sz. */
	template<typename = std::enable_if_t<detail::CanReallocate<P>::value || detail::CanReallocate<F>::value>>
	bool Reallocate(Blk& blk, size_t sz)
	{
		if(P::Owns(blk))
			return detail::ReallocateIf<P>::apply(*this, blk, sz);
		else
			return detail::ReallocateIf<F>::apply(*this, blk, sz);
	}

	/* Attempts to reallocate the memory of blk (aligned to alignment) to the new size sz. */
	template<typename = std::enable_if_t<detail::CanReallocateAligned<P>::value || detail::CanReallocateAligned<F>::value>>
	bool ReallocateAligned(Blk& blk, size_t sz, size_t alignment = 0)
	{
		if (P::Owns(blk))
			return detail::ReallocateAlignedIf<P>::apply(*this, blk, sz, (alignment == 0) ? P::Alignment : alignment);
		else
			return detail::ReallocateAlignedIf<F>::apply(*this, blk, sz, (alignment == 0) ? F::Alignment : alignment);
	}

public:
	/* Frees the memory for blk. */
	template<typename = std::enable_if_t<detail::CanDeallocate<P>::value || detail::CanDeallocate<F>::value>>
	void Deallocate(const Blk& blk)
	{
		if (P::Owns(blk))
			detail::DeallocateIf<P>::apply(*this, blk);
		else
			detail::DeallocateIf<F>::apply(*this, blk);
	}

	/* Frees the memory for blk (blk needs to have been allocated with AllocateAligned). */
	template<typename = std::enable_if_t<
		detail::CanAllocateAligned<P>::value &&
		detail::CanAllocateAligned<F>::value &&
		(detail::CanDeallocateAligned<P>::value || detail::CanDeallocateAligned<F>::value)>>
	void DeallocateAligned(const Blk& blk)
	{
		if (P::Owns(blk))
			detail::DeallocateAlignedIf<P>::apply(*this, blk);
		else
			detail::DeallocateAlignedIf<F>::apply(*this, blk);
	}

	/* Frees all of the memory in both allocators. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<P>::value && detail::CanDeallocateAll<F>::value>>
	void DeallocateAll() noexcept
	{
		P::DeallocateAll();
		F::DeallocateAll();
	}

public:
	/* Frees all of the memory in the primary allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<P>::value>>
	void DeallocateAllPrimary()
	{
		P::DeallocateAll();
	}

	/* Frees all of the memory in the fallback allocator. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<F>::value>>
	void DeallocateAllFallback()
	{
		F::DeallocateAll();
	}

private:
	void* operator new (size_t) noexcept = delete;
	void* operator new[] (size_t) noexcept = delete;
	void operator delete (void*) noexcept = delete;
	void operator delete[] (void*) noexcept = delete;
};
