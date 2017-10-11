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
#include <Epic/Memory/Default.hpp>
#include <Epic/STL/Allocator.hpp>
#include <cassert>
#include <cstdint>
#include <memory>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class CRTP>
	class CustomNew;
}

//////////////////////////////////////////////////////////////////////////////

/// CustomNew<CRTP>
template<class CRTP>
class Epic::CustomNew
{
public:
	using Type = Epic::CustomNew<CRTP>;

public:
	CustomNew() = default;

protected:
	using AllocatorType = Epic::STLAllocatorAdapted<Epic::DefaultAllocatorFor<CRTP, eAllocatorFor::New>>;

	static constexpr bool IsAligned = !detail::CanAllocate<AllocatorType>::value ||
									  (AllocatorType::Alignment % detail::AlignOf<CRTP>::value) != 0;

private:
	inline static void* _Allocate(size_t sz)
	{
		Blk blk;
		AllocatorType allocator;
		size_t alignment;

		if constexpr (IsAligned)
		{
			// Attempt to allocate aligned memory via AllocateAligned()
			static_assert(detail::CanAllocateAligned<AllocatorType>::value,
				"CustomNew::_Allocate() - This type requires an allocator that is capable of "
				"performing allocations aligned to <CRTP>");

			alignment = detail::AlignOf<CRTP>::value;
			blk = allocator.AllocateAligned(sz, alignment);
		}
		
		else
		{
			// Attempt to allocate memory via Allocate()
			static_assert(detail::CanAllocate<AllocatorType>::value,
				"CustomNew::_Allocate() - This type requires an allocator that is capable of "
				"performing allocations");

			alignment = AllocatorType::Alignment;
			blk = allocator.Allocate(sz);
		}

		// Ensure memory was acquired
		if (!blk) 
			throw std::bad_alloc{};

		// Store block size in prefix object
		auto pPrefix = allocator.Allocator().GetPrefixObject(blk, alignment);
		pPrefix->Size = blk.Size;

		return blk.Ptr;
	}

	inline static void _Deallocate(void* __restrict p)
	{
		AllocatorType allocator;

		// The AffixAllocator doesn't need to know a block's size to calculate the 
		// prefix object from a pointer.  A temporary block will be used.
		Blk blk{ p, 1 };
		
		if constexpr (IsAligned && detail::CanDeallocateAligned<AllocatorType>::value)
		{
			// AllocateAligned was used
			const auto pPrefix = allocator.Allocator().GetPrefixObject(blk, detail::AlignOf<CRTP>::value);
			blk.Size = pPrefix->Size;

			allocator.DeallocateAligned(blk);
		}
		else if constexpr (!IsAligned && detail::CanDeallocate<AllocatorType>::value)
		{
			// Allocate was used
			const auto pPrefix = allocator.Allocator().GetPrefixObject(blk);
			blk.Size = pPrefix->Size;

			allocator.Deallocate(blk);
		}
	}

public:
	// Called by the usual single-object new-expressions for allocating an object (of the derived type).
	__declspec(allocator) inline static void* operator new (size_t sz)
	{
		return _Allocate(sz);
	}

	// Called by the standard single-object placement new expression.
	inline static void* operator new (size_t /*sz*/, void* ptr) noexcept
	{
		return ptr;
	}

	// Called by the usual array new[]-expressions if allocating an array of objects (of the derived type).
	__declspec(allocator) inline static void* operator new[] (size_t sz)
	{
		return _Allocate(sz);
	}

	// Called by the standard array form placement new expression.
	inline static void* operator new[] (size_t /*sz*/, void* ptr) noexcept
	{
		return ptr;
	}

public:
	// Called by the usual single-object delete-expressions for deallocating an object (of the derived type).
	inline static void operator delete (void* pWhat)
	{
		_Deallocate(pWhat);
	}

	// Called by the standard single-object placement new expression if the object's constructor throws an exception.
	inline static void operator delete (void* /*pWhat*/, void* /*pWhere*/)
	{
		/* Do nothing */
	}

	// Called by the usual array delete[]-expressions for deallocating an array of objects (of the derived type).
	inline static void operator delete[] (void* pWhat)
	{
		_Deallocate(pWhat);
	}
};
