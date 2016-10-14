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
	template<class T>
	class CustomNew;
}

//////////////////////////////////////////////////////////////////////////////

/// CustomNew<T>
template<class T>
class Epic::CustomNew
{
public:
	using Type = Epic::CustomNew<T>;

public:
	CustomNew() = default;

private:
	inline static void* _Allocate(size_t sz)
	{
		using AllocatorType = Epic::STLAllocatorAdapted<Epic::DefaultAllocatorFor<T, eAllocatorFor::New>>;

		const bool aligned = !detail::CanAllocate<AllocatorType>::value || 
							 (AllocatorType::Alignment % alignof(T)) != 0;

		Blk blk;
		size_t alignment;
		AllocatorType allocator;

		if (aligned)
		{
			// Attempt to allocate aligned memory via AllocateAligned()
			assert(detail::CanAllocateAligned<AllocatorType>::value &&
				"CustomNew::_Allocate - This type requires an allocator that is capable of "
				"performing allocations aligned to T");

			alignment = alignof(T);
			blk = detail::AllocateAlignedIf<AllocatorType>::apply(allocator, sz, alignment);
		}
		else
		{
			// Attempt to allocate memory via Allocate()
			alignment = AllocatorType::Alignment;
			blk = detail::AllocateIf<AllocatorType>::apply(allocator, sz);
		}

		// Ensure memory was acquired
		if (!blk) throw std::bad_alloc{};

		// Store size in prefix object
		auto pPrefix = allocator.Allocator().GetPrefixObject(blk, alignment);
		pPrefix->Size = blk.Size;

		return blk.Ptr;
	}

	inline static void _Deallocate(void* p)
	{
		using AllocatorType = Epic::STLAllocatorAdapted<Epic::DefaultAllocatorFor<T, eAllocatorFor::New>>;

		const bool aligned = !detail::CanAllocate<AllocatorType>::value || 
							 (AllocatorType::Alignment % alignof(T)) != 0;

		AllocatorType allocator;

		// The AffixAllocator doesn't need to know a block's size to calculate the 
		// prefix object from a pointer.  A temporary block will be used.
		Blk blk{ p, 1 };
		
		if (aligned)
		{
			// AllocateAligned was used
			auto pPrefix = allocator.Allocator().GetPrefixObject(blk, alignof(T));
			blk.Size = pPrefix->Size;

			detail::DeallocateAlignedIf<AllocatorType>::apply(allocator, blk);
		}
		else
		{
			// Allocate was used
			auto pPrefix = allocator.Allocator().GetPrefixObject(blk);
			blk.Size = pPrefix->Size;

			detail::DeallocateIf<AllocatorType>::apply(allocator, blk);
		}
	}

public:
	// Called by the usual single-object new-expressions for allocating an object (of the derived type).
	__declspec(allocator) inline static void* operator new (std::size_t sz)
	{
		return _Allocate(sz);
	}

	// Called by the standard single-object placement new expression.
	inline static void* operator new (std::size_t /*sz*/, void* ptr) noexcept
	{
		return ptr;
	}

	// Called by the usual array new[]-expressions if allocating an array of objects (of the derived type).
	__declspec(allocator) inline static void* operator new[] (std::size_t sz)
	{
		return _Allocate(sz);
	}

	// Called by the standard array form placement new expression.
	inline static void* operator new[] (std::size_t /*sz*/, void* ptr) noexcept
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
