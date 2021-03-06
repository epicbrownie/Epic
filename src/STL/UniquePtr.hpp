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
	template<class T, class AllocatorType>
	struct DeleterFn;
	
	struct Deleter;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T>
	using UniquePtr = std::unique_ptr<T, Epic::detail::Deleter>;
}

//////////////////////////////////////////////////////////////////////////////

// DeleterFn
template<class T, class A>
struct Epic::detail::DeleterFn
{
	inline static void Delete(void* pObj, size_t extent)
	{
		using AllocatorType = Epic::STLAllocator<T, A>;
		using AllocatorTraits = std::allocator_traits<AllocatorType>;

		AllocatorType allocator;

		auto p = reinterpret_cast<T*>(pObj);

		// Destroy pObject(s)
		for (size_t i = 0; i < extent; ++i)
			AllocatorTraits::destroy(allocator, &p[i]);

		// Deallocate memory block
		AllocatorTraits::deallocate(allocator, p, extent);
	}
};

//////////////////////////////////////////////////////////////////////////////

// Deleter
struct Epic::detail::Deleter
{
	using TypeErasedDeleterFn = void(*)(void*, size_t);

	size_t m_Extent;
	TypeErasedDeleterFn m_pDeleteFn;

	Deleter() noexcept
		: m_Extent{ 0 }, m_pDeleteFn{ nullptr } 
	{ }

	Deleter(size_t extent, TypeErasedDeleterFn pfnDelete) noexcept
		: m_Extent{ extent }, m_pDeleteFn{ pfnDelete } 
	{ }

	Deleter(const Deleter& other) noexcept 
		: m_Extent{ other.m_Extent }, m_pDeleteFn{ other.m_pDeleteFn }
	{ }

	template<class T>
	inline void operator() (T* p)
	{
		assert(m_Extent > 0);
		assert(m_pDeleteFn);

		m_pDeleteFn(p, m_Extent);
	}
};

//////////////////////////////////////////////////////////////////////////////

// MakeUnique
namespace Epic
{
	/// MakeUnique<T, A, Args...>
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::UniquePtr>, class... Args>
		inline typename std::enable_if<!std::is_array<T>::value, Epic::UniquePtr<T>>::type
	MakeUnique(Args&&... args)
	{
		using AllocatorType = Epic::STLAllocator<T, A>;
		using AllocatorTraits = std::allocator_traits<AllocatorType>;

		AllocatorType allocator;

		// Use the allocator to create a T
		T* pObject = AllocatorTraits::allocate(allocator, 1);

		// Attempt to construct the T
		try
		{
			AllocatorTraits::construct(allocator, pObject, std::forward<Args>(args)...);
		}
		catch (...)
		{
			AllocatorTraits::deallocate(allocator, pObject, 1);
			throw;
		}

		// Construct the unique_ptr
		return Epic::UniquePtr<T>{ pObject, { 1, &detail::DeleterFn<T, A>::Delete } };
	}

	/// MakeUnique<T[], A>
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::UniquePtr>> 
		inline typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, 
									   Epic::UniquePtr<T>>::type
	MakeUnique(size_t Count)
	{
		using Elem = std::remove_extent_t<T>;
		using AllocatorType = Epic::STLAllocator<Elem, A>;
		using AllocatorTraits = std::allocator_traits<AllocatorType>;

		// Check for empty array
		if (Count == 0)
		{
			return std::unique_ptr<Elem[], detail::Deleter>
			{ 
				((Elem*)nullptr), 
				(detail::Deleter{ 0, nullptr }) 
			};
		}

		AllocatorType allocator;

		// Use the allocator to create the object array
		Elem* pObjects = AllocatorTraits::allocate(allocator, Count);

		// Attempt to construct the objects
		try
		{
			for (size_t i = 0; i < Count; ++i)
				AllocatorTraits::construct(allocator, &pObjects[i]);
		}
		catch (...)
		{
			AllocatorTraits::deallocate(allocator, pObjects, Count);
			throw;
		}

		// Construct the unique_ptr
		return Epic::UniquePtr<Elem[]>{ pObjects, { Count, &detail::DeleterFn<Elem, A>::Delete } };
	}

	/// MakeUnique<T, Types...>
	template<class T, class... Types>
		typename std::enable_if<std::extent<T>::value != 0, 
								struct MakeUnique_Cannot_Create_Array_With_Extent>::type
	MakeUnique(Types...) = delete;
}

//////////////////////////////////////////////////////////////////////////////

// MakeImpl
namespace Epic
{
	/// MakeImpl<B, D, A, Args...>
	template<class B, class D, class A = Epic::DefaultAllocatorFor<D, Epic::eAllocatorFor::UniquePtr>, class... Args>
		inline typename std::enable_if<!std::is_array<B>::value, Epic::UniquePtr<B>>::type
	MakeImpl(Args&&... args)
	{
		using AllocatorType = Epic::STLAllocator<D, A>;
		using AllocatorTraits = std::allocator_traits<AllocatorType>;

		AllocatorType allocator;

		// Use the allocator to create a D
		D* pObject = AllocatorTraits::allocate(allocator, 1);

		// Attempt to construct the D
		try
		{
			AllocatorTraits::construct(allocator, pObject, std::forward<Args>(args)...);
		}
		catch (...)
		{
			AllocatorTraits::deallocate(allocator, pObject, 1);
			throw;
		}

		// Construct the unique_ptr
		return Epic::UniquePtr<B>{ pObject, { 1, &detail::DeleterFn<D, A>::Delete } };
	}
}
