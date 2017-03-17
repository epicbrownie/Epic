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

	template<class T, class AllocatorType>
	struct ImplDeleterFn;

	struct ImplDeleter;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::UniquePtr>>
	using UniquePtr = std::unique_ptr<T, Epic::detail::Deleter<A>>;

	template<class T>
	using ImplPtr = std::unique_ptr<T, Epic::detail::ImplDeleter>;
}

//////////////////////////////////////////////////////////////////////////////

// ImplDeleterFn
template<class T, class A>
struct Epic::detail::ImplDeleterFn
{
	static void Apply(void* pObj, size_t extent)
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
template<class A>
struct Epic::detail::Deleter
{
	size_t m_Extent;

	Deleter() noexcept
		: m_Extent{ 0 } { }

	Deleter(const size_t extent) noexcept
		: m_Extent{ extent } { }
	
	template<class T>
	void operator() (T* p)
	{
		assert(m_Extent > 0);

		using AllocatorType = Epic::STLAllocator<T, A>;
		using AllocatorTraits = std::allocator_traits<AllocatorType>;

		AllocatorType allocator;
		
		// Destroy pObject(s)
		for (size_t i = 0; i < m_Extent; ++i)
			AllocatorTraits::destroy(allocator, &p[i]);

		// Deallocate memory block
		AllocatorTraits::deallocate(allocator, p, m_Extent);
	}
};

// ImplDeleter
struct Epic::detail::ImplDeleter
{
	using TypeErasedDeleterFn = void(*)(void*, size_t);

	size_t m_Extent;
	TypeErasedDeleterFn m_pDeleterFn;

	ImplDeleter() noexcept
		: m_Extent{ 0 }, m_pDeleterFn{ nullptr } { }

	ImplDeleter(const size_t extent, TypeErasedDeleterFn pDelete) noexcept
		: m_Extent{ extent }, m_pDeleterFn{ pDelete } { }

	template<class T>
	void operator() (T* p)
	{
		assert(m_Extent > 0);
		assert(m_pDeleterFn);

		m_pDeleterFn(p, m_Extent);
	}
};

//////////////////////////////////////////////////////////////////////////////

// MakeUnique
namespace Epic
{
	/// MakeUnique<T, A, Args...>
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::UniquePtr>, class... Args>
	inline typename std::enable_if<!std::is_array<T>::value, 
		Epic::UniquePtr<T, A>>::type
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
		return Epic::UniquePtr<T, A>{ pObject, { 1 } };
	}

	/// MakeUnique<T[], A>
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::UniquePtr>> 
	inline typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, 
		Epic::UniquePtr<T, A>>::type
	MakeUnique(const size_t Count)
	{
		using Elem = std::remove_extent_t<T>;
		using AllocatorType = Epic::STLAllocator<Elem, A>;
		using AllocatorTraits = std::allocator_traits<AllocatorType>;

		// Check for empty array
		if (Count == 0)
			return std::unique_ptr<Elem[], detail::Deleter<A>>{ ((Elem*)nullptr), (detail::Deleter<A>{ 0 }) };

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
		return std::unique_ptr<Elem[], A>{ pObjects, { Count } };
	}

	/// MakeUnique<T, Types...>
	template<class T, class... Types>
	typename std::enable_if<std::extent<T>::value != 0, struct MakeUnique_Cannot_Create_Array_With_Extent>::type
	MakeUnique(Types...) = delete;
}

//////////////////////////////////////////////////////////////////////////////

// MakeImpl
namespace Epic
{
	/// MakeImpl<B, D, A, Args...>
	template<class B, class D, class A = Epic::DefaultAllocatorFor<D, Epic::eAllocatorFor::UniquePtr>, class... Args>
	inline typename std::enable_if<!std::is_array<B>::value, 
		Epic::ImplPtr<B>>::type
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
		return Epic::ImplPtr<B>{ pObject, { 1, &detail::ImplDeleterFn<D, A>::Apply } };
	}
}
