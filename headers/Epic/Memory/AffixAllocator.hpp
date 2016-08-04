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

#include <Epic/Memory/MemoryBlock.hpp>
#include <Epic/Memory/detail/AllocatorTraits.hpp>
#include <Epic/Memory/detail/AllocatorHelpers.hpp>
#include <Epic/Memory/detail/AffixHelpers.hpp>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class Allocator, class Prefix, class Suffix = void>
	class AffixAllocator;
}

//////////////////////////////////////////////////////////////////////////////

/// AffixAllocator<A, Prefix, Suffix>
template<class A, class Prefix, class Suffix>
class Epic::AffixAllocator
{
	static_assert(std::is_default_constructible<A>::value, "The aligned allocator must be default-constructible.");
	
public:
	using type = Epic::AffixAllocator<A, Prefix, Suffix>;
	using AllocatorType = A;
	using PrefixType = Prefix;
	using SuffixType = Suffix;

public:
	static constexpr size_t Alignment = A::Alignment;
	
	static constexpr bool HasPrefix = !std::is_same<Prefix, void>::value;
	static constexpr bool HasSuffix = !std::is_same<Suffix, void>::value;

	static constexpr size_t PrefixSize = detail::RoundToAligned(detail::AffixSize<Prefix>::value, Alignment);
	static constexpr size_t SuffixSize = detail::AffixSize<Suffix>::value;
	static constexpr size_t AffixSize = PrefixSize + SuffixSize;

	static constexpr size_t MinAllocSize = A::MinAllocSize;
	static constexpr size_t MaxAllocSize = A::MaxAllocSize - AffixSize;

	static_assert(!HasPrefix || std::is_default_constructible<Prefix>::value, "The Prefix type must be default-constructible.");
	static_assert(!HasSuffix || std::is_default_constructible<Suffix>::value, "The Suffix type must be default-constructible.");

	static_assert(A::MaxAllocSize > AffixSize || MaxAllocSize > MinAllocSize, "The Affix sizes are too large for this Allocator.");

private:
	AllocatorType m_Allocator;
	
public:
	constexpr AffixAllocator()
		noexcept(std::is_nothrow_default_constructible<A>::value) = default;

	template<typename = std::enable_if_t<std::is_copy_constructible<A>::value>>
	constexpr AffixAllocator(const type& obj)
		noexcept(std::is_nothrow_copy_constructible<A>::value)
		: m_Allocator{ obj.m_Allocator }
	{ }

	template<typename = std::enable_if_t<std::is_move_constructible<A>::value>>
	constexpr AffixAllocator(type&& obj)
		noexcept(std::is_nothrow_move_constructible<A>::value)
		: m_Allocator{ std::move(obj.m_Allocator) }
	{ }

	template<typename = std::enable_if_t<std::is_copy_assignable<A>::value>>
	AffixAllocator& operator = (const type& obj)
		noexcept(std::is_nothrow_copy_assignable<A>::value)
	{
		m_Allocator = obj.m_Allocator;

		return *this;
	}

	template<typename = std::enable_if_t<std::is_move_assignable<A>::value>>
	AffixAllocator& operator = (type&& obj)
		noexcept(std::is_nothrow_move_assignable<A>::value)
	{
		m_Allocator = std::move(obj.m_Allocator);

		return *this;
	}

private:
	static constexpr Blk ClientToAffixedBlock(const Blk& blk) noexcept
	{
		return Blk
		{
			reinterpret_cast<char*>(blk.Ptr) - PrefixSize,
			blk.Size + PrefixSize + SuffixSize
		};
	}

	static constexpr Blk AffixedToClientBlock(const Blk& blk) noexcept
	{
		return Blk
		{
			reinterpret_cast<char*>(blk.Ptr) + PrefixSize,
			blk.Size - PrefixSize - SuffixSize
		};
	}

	static constexpr void* AffixedToPrefixPtr(const Blk& blk) noexcept
	{
		return blk.Ptr;
	}

	static constexpr void* AffixedToSuffixPtr(const Blk& blk) noexcept
	{
		return static_cast<void*>(reinterpret_cast<char*>(blk.Ptr) + blk.Size - SuffixSize);
	}

	static constexpr void* ClientToPrefixPtr(const Blk& blk) noexcept
	{
		return static_cast<void*>(reinterpret_cast<char*>(blk.Ptr) - PrefixSize);
	}

	static constexpr void* ClientToSuffixPtr(const Blk& blk) noexcept
	{
		return static_cast<void*>(reinterpret_cast<char*>(blk.Ptr) + blk.Size);
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	inline bool Owns(const Blk& blk) const noexcept
	{
		return m_Allocator.Owns(blk ? ClientToAffixedBlock(blk) : blk);
	}

public:
	/* Returns a block of uninitialized memory.
	   The memory will be surrounded by constructed Affix objects. */
	template<typename = std::enable_if_t<detail::CanAllocate<A>::value>>
	Blk Allocate(size_t sz) noexcept
	{
		// Verify that the requested size isn't zero.
		if (sz == 0) return{ nullptr, 0 };
		
		size_t newsz = sz + AffixSize;

		// Verify that the requested size is within our allowed bounds
		if (newsz < MinAllocSize || newsz > MaxAllocSize)
			return{ nullptr, 0 };

		// Allocate the block
		auto blk = m_Allocator.Allocate(newsz);
		if (!blk) return{ nullptr, 0 };

		// Construct the Prefix and Suffix objects
		detail::AffixConstructor<Prefix>::apply(AffixedToPrefixPtr(blk));
		detail::AffixConstructor<Suffix>::apply(AffixedToSuffixPtr(blk));

		return AffixedToClientBlock(blk);
	}

	/* Attempts to reallocate the memory of blk to the new size sz.
	   The Affix objects will be moved as necessary. */
	template<typename = std::enable_if_t<detail::CanReallocate<A>::value && detail::AffixBuffer<Suffix>::CanStore>>
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

		size_t newsz = sz + AffixSize;

		// Verify that the requested size is within our allowed bounds
		if (newsz < MinAllocSize || newsz > MaxAllocSize)
			return false;

		// Move the Suffix object to the stack
		auto pSuffix = GetSuffixObject(blk);
		detail::AffixBuffer<Suffix> suffix{ pSuffix };

		// Reallocate the block
		Blk affixedBlk = ClientToAffixedBlock(blk);

		if (!m_Allocator.Reallocate(affixedBlk, newsz))
		{
			suffix.Restore(pSuffix);
			return false;
		}

		// Place the Suffix object and return
		suffix.Restore(AffixedToSuffixPtr(affixedBlk));
		blk = AffixedToClientBlock(affixedBlk);

		return true;
	}

public:
	/* Frees the memory for blk.
	   The surrounding Affix objects will also be destroyed. */
	template<typename = std::enable_if_t<detail::CanDeallocate<A>::value>>
	void Deallocate(const Blk& blk)
	{
		if (!blk) return;
		assert(Owns(blk) && "AffixAllocator::Deallocate - Attempted to free a block that was not allocated by this allocator");

		// Deconstruct the affix objects
		if (HasPrefix) GetPrefixObject(blk)->~Prefix();
		if (HasSuffix) GetSuffixObject(blk)->~Suffix();

		// Deallocate the affixed block		
		m_Allocator.Deallocate(ClientToAffixedBlock(blk));
	}

public:
	static constexpr Prefix* GetPrefixObject(const Blk& blk) noexcept
	{
		return HasPrefix ? reinterpret_cast<Prefix*>(ClientToPrefixPtr(blk)) : nullptr;
	}

	static constexpr Suffix* GetSuffixObject(const Blk& blk) noexcept
	{
		return HasSuffix ? reinterpret_cast<Suffix*>(ClientToSuffixPtr(blk)) : nullptr;
	}
};
