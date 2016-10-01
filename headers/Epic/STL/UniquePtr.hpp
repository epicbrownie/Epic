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

#include <Epic/Memory/Default.hpp>
#include <Epic/STL/Allocator.hpp>
#include <memory>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class AllocatorType>
	struct Deleter;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::UniquePtr>>
	using UniquePtr = std::unique_ptr<T, Epic::detail::Deleter<A>>;
}

//////////////////////////////////////////////////////////////////////////////

template<class A>
struct Epic::detail::Deleter
{
	size_t m_Extent;

	Deleter() noexcept
		: m_Extent{ 0 } { }

	Deleter(size_t extent) noexcept
		: m_Extent{ extent } { }
	
	template<class T>
	void operator() (T* p)
	{
		assert(m_Extent > 0);

		Epic::STLAllocator<T, A> allocator;
		
		// Destroy pObject(s)
		for (size_t i = 0; i < m_Extent; ++i)
			allocator.destroy(&p[i]);

		// Deallocate memory block
		allocator.deallocate(p, m_Extent);
	}
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// MakeUnique<T, A, Args...>
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::UniquePtr>, class... Args>
	inline typename std::enable_if<!std::is_array<T>::value, 
		Epic::UniquePtr<T, A>>::type
	MakeUnique(Args&&... args)
	{
		using AllocatorType = Epic::STLAllocator<T, A>;
		AllocatorType allocator;

		// Use the allocator to create a T
		T* pObject = allocator.allocate(1);

		// Attempt to construct the T
		try
		{
			allocator.construct(pObject, std::forward<Args>(args)...);
		}
		catch (...)
		{
			allocator.deallocate(pObject, 1);
			throw;
		}

		// Construct the unique_ptr
		return std::unique_ptr<T, detail::Deleter<A>>{ pObject, { 1 } };
	}

	/// MakeUnique<T[], A>
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::UniquePtr>> 
	inline typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, 
		Epic::UniquePtr<T, A>>::type
	MakeUnique(size_t Count)
	{
		using Elem = std::remove_extent_t<T>;
		using AllocatorType = Epic::STLAllocator<Elem, A>;
		
		// Check for empty array
		if (Count == 0)
			return std::unique_ptr<Elem[], detail::Deleter<A>>{ ((Elem*)nullptr), (detail::Deleter<A>{ 0 }) };

		AllocatorType allocator;

		// Use the allocator to create the object array
		Elem* pObjects = allocator.allocate(Count);

		// Attempt to construct the objects
		try
		{
			for (size_t i = 0; i < Count; ++i)
				allocator.construct(&pObjects[i]);
		}
		catch (...)
		{
			allocator.deallocate(pObjects, Count);
			throw;
		}

		// Construct the unique_ptr
		return std::unique_ptr<Elem[], detail::Deleter<A>>{ pObjects, { Count } };
	}

	/// MakeUnique<T, Types...>
	template<class T, class... Types>
	typename std::enable_if<std::extent<T>::value != 0, struct MakeUnique_Cannot_Create_Array_With_Extent>::type
	MakeUnique(Types...) = delete;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// MakeImpl<B, D, A, Args...>
	template<class B, class D, class A = Epic::DefaultAllocatorFor<D, Epic::eAllocatorFor::UniquePtr>, class... Args>
	inline typename std::enable_if<!std::is_array<B>::value, 
		Epic::UniquePtr<B, A>>::type
	MakeImpl(Args&&... args)
	{
		using AllocatorType = Epic::STLAllocator<D, A>;
		AllocatorType allocator;

		// Use the allocator to create a D
		D* pObject = allocator.allocate(1);

		// Attempt to construct the D
		try
		{
			allocator.construct(pObject, std::forward<Args>(args)...);
		}
		catch (...)
		{
			allocator.deallocate(pObject, 1);
			throw;
		}

		// Construct the unique_ptr
		return std::unique_ptr<B, detail::Deleter<A>>{ pObject, { 1 } };
	}
}
