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
	static_assert(std::is_default_constructible<A>::value, "The affix allocator must be default-constructible.");
	
public:
	using Type = Epic::AffixAllocator<A, Prefix, Suffix>;
	using AllocatorType = A;
	using PrefixType = Prefix;
	using SuffixType = Suffix;

public:
	static constexpr size_t Alignment = A::Alignment;
	
private:
	static constexpr bool HasPrefix = !std::is_same<Prefix, void>::value;
	static constexpr bool HasSuffix = !std::is_same<Suffix, void>::value;

	static constexpr size_t UnalignedPrefixSize = detail::AffixSize<Prefix>::value;
	static constexpr size_t PrefixSize = detail::RoundToAligned(UnalignedPrefixSize, Alignment);
	static constexpr size_t SuffixSize = detail::AffixSize<Suffix>::value;
	
	using AlignmentMemento = uint16_t;

private:
	static constexpr size_t NonAllocSize = (PrefixSize + SuffixSize + sizeof(AlignmentMemento));

public:
	static constexpr size_t MinAllocSize = A::MinAllocSize;
	static constexpr size_t MaxAllocSize = A::MaxAllocSize - NonAllocSize;
	static constexpr bool IsShareable = A::IsShareable;

	static_assert(!HasPrefix || std::is_default_constructible<Prefix>::value, "The Prefix Type must be default-constructible.");
	static_assert(!HasSuffix || std::is_default_constructible<Suffix>::value, "The Suffix Type must be default-constructible.");

	static_assert(A::MaxAllocSize > NonAllocSize && MaxAllocSize > MinAllocSize, 
		"The affix sizes are too large for the backing Allocator.");

private:
	AllocatorType m_Allocator;
	
public:
	constexpr AffixAllocator()
		noexcept(std::is_nothrow_default_constructible<A>::value) = default;

	template<typename = std::enable_if_t<std::is_copy_constructible<A>::value>>
	constexpr AffixAllocator(const Type& obj)
		noexcept(std::is_nothrow_copy_constructible<A>::value)
		: m_Allocator{ obj.m_Allocator }
	{ }

	template<typename = std::enable_if_t<std::is_move_constructible<A>::value>>
	constexpr AffixAllocator(Type&& obj)
		noexcept(std::is_nothrow_move_constructible<A>::value)
		: m_Allocator{ std::move(obj.m_Allocator) }
	{ }

	template<typename = std::enable_if_t<std::is_copy_assignable<A>::value>>
	AffixAllocator& operator = (const Type& obj)
		noexcept(std::is_nothrow_copy_assignable<A>::value)
	{
		m_Allocator = obj.m_Allocator;

		return *this;
	}

	template<typename = std::enable_if_t<std::is_move_assignable<A>::value>>
	AffixAllocator& operator = (Type&& obj)
		noexcept(std::is_nothrow_move_assignable<A>::value)
	{
		m_Allocator = std::move(obj.m_Allocator);

		return *this;
	}

private:
	static constexpr Blk ClientToAffixedBlock(const Blk& blk, const AlignmentMemento& alignment) noexcept
	{
		return Blk
		{
			reinterpret_cast<char*>(blk.Ptr) - detail::RoundToAligned(UnalignedPrefixSize, alignment),
			blk.Size + (detail::RoundToAligned(UnalignedPrefixSize, alignment) + SuffixSize + sizeof(AlignmentMemento))
		};
	}

	static constexpr Blk AffixedToClientBlock(const Blk& blk, const AlignmentMemento& alignment) noexcept
	{
		return Blk
		{
			reinterpret_cast<char*>(blk.Ptr) + detail::RoundToAligned(UnalignedPrefixSize, alignment),
			blk.Size - (detail::RoundToAligned(UnalignedPrefixSize, alignment) + SuffixSize + sizeof(AlignmentMemento))
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

	static constexpr AlignmentMemento* AffixedToAlignmentMementoPtr(const Blk& blk)
	{
		return reinterpret_cast<AlignmentMemento*>(reinterpret_cast<char*>(AffixedToSuffixPtr(blk)) - sizeof(AlignmentMemento));
	}

	static constexpr void* ClientToPrefixPtr(const Blk& blk, const AlignmentMemento& alignment) noexcept
	{
		return static_cast<void*>(reinterpret_cast<char*>(blk.Ptr) - detail::RoundToAligned(UnalignedPrefixSize, alignment));
	}

	static constexpr void* ClientToSuffixPtr(const Blk& blk) noexcept
	{
		return static_cast<void*>(reinterpret_cast<char*>(blk.Ptr) + blk.Size + sizeof(AlignmentMemento));
	}

	static constexpr AlignmentMemento* ClientToAlignmentMementoPtr(const Blk& blk)
	{
		return reinterpret_cast<AlignmentMemento*>(reinterpret_cast<char*>(blk.Ptr) + blk.Size);
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	inline bool Owns(const Blk& blk) const noexcept
	{
		return blk ? m_Allocator.Owns(ClientToAffixedBlock(blk, *ClientToAlignmentMementoPtr(blk))) : false;
	}

public:
	/* Returns a block of uninitialized memory.
	   The memory will be surrounded by constructed Affix objects. */
	template<typename = std::enable_if_t<detail::CanAllocate<A>::value>>
	Blk Allocate(const size_t sz) noexcept
	{
		// Verify that the requested size isn't zero.
		if (sz == 0) return{ nullptr, 0 };
		
		// Verify that the requested size is within our allowed bounds
		if (sz < MinAllocSize || sz > MaxAllocSize)
			return{ nullptr, 0 };
		
		// Allocate the block
		auto blk = m_Allocator.Allocate(sz + NonAllocSize);
		if (!blk) return{ nullptr, 0 };

		// Construct the Prefix and Suffix objects
		detail::AffixConstructor<Prefix>::apply(AffixedToPrefixPtr(blk));
		detail::AffixConstructor<Suffix>::apply(AffixedToSuffixPtr(blk));

		// Store alignment memento
		const auto memento = static_cast<AlignmentMemento>(Alignment);
		*AffixedToAlignmentMementoPtr(blk) = memento;

		return AffixedToClientBlock(blk, memento);
	}

	/* Returns a block of uninitialized memory (aligned to 'alignment').
	   The memory will be surrounded by constructed Affix objects. */
	template<typename = std::enable_if_t<detail::CanAllocateAligned<A>::value>>
	Blk AllocateAligned(const size_t sz, const size_t alignment = Alignment) noexcept
	{
		// Verify that the alignment is acceptable
		if (!detail::IsGoodAlignment(alignment))
			return{ nullptr, 0 };

		assert(alignment <= std::numeric_limits<AlignmentMemento>::max() &&
			"AffixAllocator::AllocateAligned - Unsupported alignment value");

		// Verify that the requested size isn't zero.
		if (sz == 0) return{ nullptr, 0 };

		// Verify that the requested size is within our allowed bounds
		if (sz < MinAllocSize || sz > MaxAllocSize)
			return{ nullptr, 0 };

		// Allocate the block
		const size_t szNew = sz + detail::RoundToAligned(UnalignedPrefixSize, alignment) + sizeof(AlignmentMemento) + SuffixSize;
		auto blk = m_Allocator.AllocateAligned(szNew, alignment);
		if (!blk) return{ nullptr, 0 };

		// Construct the Prefix and Suffix objects
		detail::AffixConstructor<Prefix>::apply(AffixedToPrefixPtr(blk));
		detail::AffixConstructor<Suffix>::apply(AffixedToSuffixPtr(blk));

		// Store alignment memento
		const auto memento = static_cast<AlignmentMemento>(alignment);
		*AffixedToAlignmentMementoPtr(blk) = memento;

		return AffixedToClientBlock(blk, memento);
	}

	/* Attempts to reallocate the memory of blk to the new size sz.
	   The Affix objects will be moved as necessary. */
	template<typename = std::enable_if_t<detail::CanReallocate<A>::value && detail::AffixBuffer<Suffix>::CanStore>>
	bool Reallocate(Blk& blk, const size_t sz)
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

		// Move the Suffix object to the stack
		auto pSuffix = GetSuffixObject(blk);
		detail::AffixBuffer<Suffix> suffix{ pSuffix };

		// Reallocate the block
		Blk affixedBlk = ClientToAffixedBlock(blk, Alignment);

		if (!m_Allocator.Reallocate(affixedBlk, sz + NonAllocSize))
		{
			suffix.Restore(pSuffix);
			return false;
		}

		// Place the Suffix object and the alignment memento
		suffix.Restore(AffixedToSuffixPtr(affixedBlk));
		*AffixedToAlignmentMementoPtr(affixedBlk) = Alignment;
		
		blk = AffixedToClientBlock(affixedBlk, Alignment);

		return true;
	}

	/* Attempts to reallocate the memory of blk to the new size 'sz' (aligned to 'alignment').
	   It must have been allocated through AllocateAligned().
	   The Affix objects will be moved as necessary. */
	template<typename = std::enable_if_t<detail::CanReallocateAligned<A>::value && detail::AffixBuffer<Suffix>::CanStore>>
	bool ReallocateAligned(Blk& blk, const size_t sz, const size_t alignment = Alignment)
	{
		// Verify that the alignment is acceptable
		if (!detail::IsGoodAlignment(alignment))
			return false;

		assert(alignment <= std::numeric_limits<AlignmentMemento>::max() &&
			"AffixAllocator::ReallocateAligned - Unsupported alignment value");

		// If the block isn't valid, delegate to AllocateAligned
		if (!blk)
		{
			blk = detail::AllocateAlignedIf<Type>::apply(*this, sz, alignment);
			return (bool)blk;
		}

		// If the requested size is zero, delegate to DeallocateAligned
		if (sz == 0)
		{
			if (detail::CanDeallocateAligned<Type>::value)
			{
				detail::DeallocateAlignedIf<Type>::apply(*this, blk);
				blk = { nullptr, 0 };
			}

			return detail::CanDeallocate<Type>::value;
		}

		// Verify that the requested size is within our allowed bounds
		if (sz < MinAllocSize || sz > MaxAllocSize)
			return false;

		// Verify alignment memento
		const AlignmentMemento memento = *ClientToAlignmentMementoPtr(blk);
		assert(detail::IsGoodAlignment(memento) &&
			"AffixAllocator::ReallocateAligned - Either this block was not allocated aligned or the heap has been corrupted");
		assert(alignment == memento &&
			"AffixAllocator::ReallocateAligned - Once allocated, the alignment of an allocated block cannot be changed");

		// Move the Suffix object to the stack
		auto pSuffix = GetSuffixObject(blk);
		detail::AffixBuffer<Suffix> suffix{ pSuffix };

		// Reallocate the block
		Blk affixedBlk = ClientToAffixedBlock(blk, memento);
		size_t szNew = sz + detail::RoundToAligned(UnalignedPrefixSize, alignment) + sizeof(AlignmentMemento) + SuffixSize;

		if (!m_Allocator.ReallocateAligned(affixedBlk, szNew))
		{
			suffix.Restore(pSuffix);
			return false;
		}

		// Place the Suffix object and the alignment memento
		suffix.Restore(AffixedToSuffixPtr(affixedBlk));
		*AffixedToAlignmentMementoPtr(affixedBlk) = memento;
		
		blk = AffixedToClientBlock(affixedBlk, memento);

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
		m_Allocator.Deallocate(ClientToAffixedBlock(blk, static_cast<AlignmentMemento>(Alignment)));
	}

	/* Frees the memory for blk. It must have been allocated through AllocateAligned().
	   The surrounding Affix objects will also be destroyed. */
	template<typename = std::enable_if_t<detail::CanDeallocateAligned<A>::value>>
	void DeallocateAligned(const Blk& blk)
	{
		if (!blk) return;
		assert(Owns(blk) && "AffixAllocator::DeallocateAligned - Attempted to free a block that was not allocated by this allocator");
		
		// Verify alignment memento
		assert(detail::IsGoodAlignment(*ClientToAlignmentMementoPtr(blk)) && 
			"AffixAllocator::DeallocateAligned - Either this block was not allocated aligned or the heap has been corrupted");

		// Deconstruct the affix objects
		if (HasPrefix) GetPrefixObject(blk)->~Prefix();
		if (HasSuffix) GetSuffixObject(blk)->~Suffix();

		// Deallocate the affixed block
		m_Allocator.DeallocateAligned(ClientToAffixedBlock(blk, *ClientToAlignmentMementoPtr(blk)));
	}

public:
	static constexpr Prefix* GetPrefixObject(const Blk& blk, size_t alignment = Alignment) noexcept
	{
		return HasPrefix ? 
			reinterpret_cast<Prefix*>(ClientToPrefixPtr(blk, static_cast<AlignmentMemento>(alignment))) : 
			nullptr;
	}

	static constexpr Suffix* GetSuffixObject(const Blk& blk) noexcept
	{
		return HasSuffix ? reinterpret_cast<Suffix*>(ClientToSuffixPtr(blk)) : nullptr;
	}
};
