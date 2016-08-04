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

#include <Epic/Singleton.hpp>
#include <Epic/Memory/detail/AllocatorTraits.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <cstdint>
#include <functional>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class Allocator>
	class GlobalAllocator;
}

//////////////////////////////////////////////////////////////////////////////

/// GlobalAllocator<A>
template<class A>
class Epic::GlobalAllocator
{
	static_assert(std::is_default_constructible<A>::value, "Only default-constructible allocators can be made global.");

public:
	using type = Epic::GlobalAllocator<A>;
	
public:
	static constexpr size_t Alignment = A::Alignment;
	static constexpr size_t MinAllocSize = A::MinAllocSize;
	static constexpr size_t MaxAllocSize = A::MaxAllocSize;

private:
	std::reference_wrapper<A> m_Allocator;

public:
	constexpr GlobalAllocator() noexcept
		: m_Allocator{ Epic::Singleton<A>::Instance() } { }

	constexpr GlobalAllocator(const type& obj) = default;
	constexpr GlobalAllocator(type&& obj) = default;

	GlobalAllocator& operator = (const type& obj) = default;
	GlobalAllocator& operator = (type&& obj) = default;

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	inline bool Owns(const Blk& blk) const noexcept
	{
		return m_Allocator.get().Owns(blk);
	}

public:
	/* Returns a block of uninitialized memory. */
	template<typename = std::enable_if_t<detail::CanAllocate<A>::value>>
	Blk Allocate(size_t sz) noexcept
	{
		return m_Allocator.get().Allocate(sz);
	}

	/* Returns a block of uninitialized memory (aligned to alignment). */
	template<typename = std::enable_if_t<detail::CanAllocateAligned<A>::value>>
	Blk AllocateAligned(size_t sz, size_t alignment = A::Alignment) noexcept
	{
		return m_Allocator.get().AllocateAligned(sz, alignment);
	}

	/* Attempts to reallocate the memory of blk to the new size sz. */
	template<typename = std::enable_if_t<detail::CanReallocate<A>::value>>
	bool Reallocate(Blk& blk, size_t sz)
	{
		return m_Allocator.get().Reallocate(blk, sz);
	}

	/* Attempts to reallocate the memory of blk (aligned to alignment) to the new size sz. */
	template<typename = std::enable_if_t<detail::CanReallocateAligned<A>::value>>
	bool ReallocateAligned(Blk& blk, size_t sz, size_t alignment = A::Alignment)
	{
		return m_Allocator.get().ReallocateAligned(blk, sz, alignment);
	}

	/* Returns a block of uninitialized memory. */
	template<typename = std::enable_if_t<detail::CanAllocateAll<A>::value>>
	Blk AllocateAll() noexcept
	{
		return m_Allocator.get().AllocateAll();
	}

public:
	/* Frees the memory for blk. */
	template<typename = std::enable_if_t<detail::CanDeallocate<A>::value>>
	void Deallocate(const Blk& blk)
	{
		m_Allocator.get().Deallocate(blk);
	}

	/* Frees the memory for blk. */
	template<typename = std::enable_if_t<detail::CanDeallocateAligned<A>::value>>
	void DeallocateAligned(const Blk& blk)
	{
		m_Allocator.get().DeallocateAligned(blk);
	}

	/* Frees all of the allocator's memory. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<A>::value>>
	void DeallocateAll() noexcept
	{
		m_Allocator.get().DeallocateAll();
	}
};
