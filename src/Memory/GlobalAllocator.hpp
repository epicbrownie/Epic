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

#include <Epic/Memory/detail/GlobalHelpers.hpp>
#include <Epic/Memory/detail/AllocatorTraits.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <Epic/Singleton.hpp>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	struct GTag;

	template<class A, class Tag>
	struct GlobalAllocatorAdaptor;

	template<class Allocator, class Tag>
	class GlobalAllocatorImpl;
}

//////////////////////////////////////////////////////////////////////////////

/// GlobalAllocatorImpl<A, Tag_>
template<class A, class Tag_>
class Epic::detail::GlobalAllocatorImpl
{
	static_assert(std::is_default_constructible<A>::value, "Only default-constructible allocators can be made global.");

public:
	using Type = Epic::detail::GlobalAllocatorImpl<A, Tag_>;
	using AllocatorType = A;
	using Tag = Tag_;

public:
	static constexpr size_t Alignment = A::Alignment;
	static constexpr size_t MinAllocSize = A::MinAllocSize;
	static constexpr size_t MaxAllocSize = A::MaxAllocSize;
	static constexpr bool IsShareable = A::IsShareable;

private:
	using SingletonAllocatorType = Epic::Singleton<A, Tag>;

private:
	A* m_pAllocator;

public:
	constexpr GlobalAllocatorImpl() noexcept
		: m_pAllocator{ &Type::Allocator() } 
	{ }

	constexpr GlobalAllocatorImpl(const Type& obj) = default;
	constexpr GlobalAllocatorImpl(Type&& obj) = default;

	GlobalAllocatorImpl& operator = (const Type& obj) = default;
	GlobalAllocatorImpl& operator = (Type&& obj) = default;

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	inline bool Owns(const Blk& blk) const noexcept
	{
		return m_pAllocator->Owns(blk);
	}

public:
	/* Returns a block of uninitialized memory. */
	template<typename = std::enable_if_t<detail::CanAllocate<A>::value>>
	Blk Allocate(size_t sz) noexcept
	{
		return m_pAllocator->Allocate(sz);
	}

	/* Returns a block of uninitialized memory (aligned to alignment). */
	template<typename = std::enable_if_t<detail::CanAllocateAligned<A>::value>>
	Blk AllocateAligned(size_t sz, size_t alignment = A::Alignment) noexcept
	{
		return m_pAllocator->AllocateAligned(sz, alignment);
	}

	/* Attempts to reallocate the memory of blk to the new size sz. */
	template<typename = std::enable_if_t<detail::CanReallocate<A>::value>>
	bool Reallocate(Blk& blk, size_t sz)
	{
		return m_pAllocator->Reallocate(blk, sz);
	}

	/* Attempts to reallocate the memory of blk (aligned to alignment) to the new size sz. */
	template<typename = std::enable_if_t<detail::CanReallocateAligned<A>::value>>
	bool ReallocateAligned(Blk& blk, size_t sz, size_t alignment = A::Alignment)
	{
		return m_pAllocator->ReallocateAligned(blk, sz, alignment);
	}

	/* Returns a block of uninitialized memory. */
	template<typename = std::enable_if_t<detail::CanAllocateAll<A>::value>>
	Blk AllocateAll() noexcept
	{
		return m_pAllocator->AllocateAll();
	}

public:
	/* Frees the memory for blk. */
	template<typename = std::enable_if_t<detail::CanDeallocate<A>::value>>
	void Deallocate(const Blk& blk)
	{
		m_pAllocator->Deallocate(blk);
	}

	/* Frees the memory for blk. */
	template<typename = std::enable_if_t<detail::CanDeallocateAligned<A>::value>>
	void DeallocateAligned(const Blk& blk)
	{
		m_pAllocator->DeallocateAligned(blk);
	}

	/* Frees all of the allocator's memory. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<A>::value>>
	void DeallocateAll() noexcept
	{
		m_pAllocator->DeallocateAll();
	}

public:
	static A& Allocator() noexcept
	{
		return SingletonAllocatorType::Instance();
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class A, class Tag>
struct Epic::detail::GlobalAllocatorAdaptor
{
	using type = detail::GlobalAllocatorImpl<A, Tag>;
};

template<class A, class Tag, class OldTag>
struct Epic::detail::GlobalAllocatorAdaptor<Epic::detail::GlobalAllocatorImpl<A, OldTag>, Tag>
{
	using type = Epic::GlobalAllocatorImpl<typename detail::UnwrapGlobal<A>::type, OldTag>;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		using GlobalAllocatorTag = GTag;
	}

	template<class Allocator, class Tag = Epic::detail::GlobalAllocatorTag>
	using GlobalAllocator = typename detail::GlobalAllocatorAdaptor<Allocator, Tag>::type;
}
