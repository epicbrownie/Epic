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

#include <Epic/Memory/detail/AllocatorHelpers.hpp>
#include <Epic/Memory/Default.hpp>
#include <Epic/Memory/GlobalAllocator.hpp>
#include <Epic/STL/detail/UniqueHelpers.hpp>
#include <memory>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	struct Deleter;
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::detail::Deleter
{
	using DeallocateFn = void(*)(const Blk&);
	
	DeallocateFn m_pDeallocateFn;
	Blk m_Block;
	size_t m_Extent;

	// Prevent user from accidentally using Deleter in a std::unique_ptr 
	// that was not created through Epic::MakeUnique().
	Deleter() = delete;

	template<class A, typename EnabledForGlobalAllocators = std::enable_if_t<detail::IsGlobal<A>::value>>
	Deleter(const A&, const Blk& blk, bool aligned, size_t extent = 1) noexcept
		: m_Block{ blk }, 
		  m_pDeallocateFn
		  { aligned ? (&detail::UniqueDeallocator<A>::DeallocateAligned) :
					  (&detail::UniqueDeallocator<A>::Deallocate)
		  }, 
		  m_Extent{ extent }
	{ }
	
	template<typename T>
	void operator() (T* pObject)
	{
		// Destroy pObject(s)
		for(size_t i=0; i<m_Extent; ++i)
			pObject[i].~T();

		// Deallocate memory block
		if (m_pDeallocateFn)
			m_pDeallocateFn(m_Block);
	}
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// MakeUnique<T, A, Args...>
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::UniquePtr>, class... Args>
	inline typename std::enable_if_t<!std::is_array<T>::value, 
		std::unique_ptr<T, Epic::detail::Deleter>>
	MakeUnique(Args&&... args)
	{
		using AllocatorType = GlobalAllocator<A>;

		// Allocate memory
		AllocatorType allocator;
		Blk blk;
		bool allocAligned = !detail::CanAllocate<AllocatorType>::value || (AllocatorType::Alignment % alignof(T)) != 0;
		
		if (allocAligned)
		{
			assert(detail::CanAllocateAligned<AllocatorType>::value &&
				"MakeUnique<T>() - This type requires an allocator that is capable of "
				"performing arbitrarily aligned allocations");

			// Attempt to allocate aligned memory via AllocateAligned()
			blk = detail::AllocateAlignedIf<AllocatorType>::apply(allocator, sizeof(T), alignof(T));
		}
		else
		{
			// Attempt to allocate memory via Allocate()
			blk = detail::AllocateIf<AllocatorType>::apply(allocator, sizeof(T));
		}

		// Ensure memory was acquired
		if (!blk) throw std::bad_alloc{};

		// Construct the object
		T* pObject = nullptr;

		try
		{
			pObject = ::new (blk.Ptr) T(std::forward<Args>(args)...);
		}
		catch (...)
		{
			// Failed to construct object. Deallocate the memory.
			if(allocAligned)
				detail::DeallocateAlignedIf<AllocatorType>::apply(allocator, blk);
			else
				detail::DeallocateIf<AllocatorType>::apply(allocator, blk);

			throw;
		}

		// Construct the unique_ptr
		return std::unique_ptr<T, Epic::detail::Deleter>{ pObject, { allocator, blk, allocAligned } };
	}

	/// MakeUnique<T[], A>
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::UniquePtr>> 
	inline typename std::enable_if_t<std::is_array<T>::value && std::extent<T>::value == 0, 
		std::unique_ptr<T, Epic::detail::Deleter>> 
	MakeUnique(size_t Count)
	{
		using Elem = std::remove_extent_t<T>;
		using AllocatorType = GlobalAllocator<A>;

		// Allocate memory
		AllocatorType allocator;
		Blk blk;
		bool allocAligned = !detail::CanAllocate<AllocatorType>::value || (AllocatorType::Alignment % alignof(Elem)) != 0;

		if (allocAligned)
		{
			assert(detail::CanAllocateAligned<AllocatorType>::value &&
				"MakeUnique<T[]>() - This type requires an allocator that is capable of "
				"performing arbitrarily aligned allocations");

			// Attempt to allocate aligned memory via AllocateAligned()
			blk = detail::AllocateAlignedIf<AllocatorType>::apply(allocator, sizeof(Elem) * Count, alignof(Elem));
		}
		else
		{
			// Attempt to allocate memory via Allocate()
			blk = detail::AllocateIf<AllocatorType>::apply(allocator, sizeof(Elem) * Count);
		}

		// Ensure memory was acquired
		if (!blk) throw std::bad_alloc{};

		// Construct the objects
		Elem* pObject = reinterpret_cast<Elem*>(blk.Ptr);

		try
		{
			for (size_t i = 0; i < Count; ++i)
			{
				::new (&pObject[i]) Elem;
			}
		}
		catch (...)
		{
			// Failed to construct objects. Deallocate the memory.
			if (allocAligned)
				detail::DeallocateAlignedIf<AllocatorType>::apply(allocator, blk);
			else
				detail::DeallocateIf<AllocatorType>::apply(allocator, blk);
			
			throw;
		}

		// Construct the unique_ptr
		return std::unique_ptr<Elem[], Epic::detail::Deleter>{ pObject, { allocator, blk, allocAligned, Count } };
	}

	/// MakeUnique<T, Types...>
	template<class T, class... Types>
	typename std::enable_if_t<std::extent<T>::value != 0, struct MakeUnique_Cannot_Create_Array_With_Extent>
	MakeUnique(Types...) = delete;
}
