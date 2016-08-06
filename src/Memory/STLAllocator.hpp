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
#include <Epic/Memory/detail/STLHelpers.hpp>
#include <memory>
#include <new>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class T, class A>
	class STLAllocatorImpl;
}

//////////////////////////////////////////////////////////////////////////////

/// STLAllocatorImpl<T, A>
template<class T, class A>
class Epic::detail::STLAllocatorImpl
{
	static_assert(!std::is_const<T>::value, "The C++ Standard forbids containers of const elements.");
	
public:
	using type = Epic::detail::STLAllocatorImpl<T, A>;
	using AllocatorType = A;

public:
	using value_type = T;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;

private:
	AllocatorType m_Allocator;

public:
	using propagate_on_container_copy_assignment = std::true_type;
	using propagate_on_container_move_assignment = std::true_type;
	using propagate_on_container_swap = std::true_type;
	using is_always_equal = std::false_type;

public:
	template<class U>
	struct rebind
	{
		using other = Epic::detail::STLAllocatorImpl<U, A>;
	};

public:
	STLAllocatorImpl() noexcept { }

	STLAllocatorImpl(const type& obj) noexcept { }

	template<class U>
	STLAllocatorImpl(const STLAllocatorImpl<U, A>&) noexcept
	{ /* Allocator should not be copied */ }

	template<class U>
	STLAllocatorImpl<T, A>& operator = (const STLAllocatorImpl<U, A>&) noexcept
	{
		/* Allocator should not be copied */
		return (*this);
	}

public:
	constexpr size_t max_size() const noexcept
	{
		// Maximum array elements
		return (AllocatorType::MaxAllocSize / sizeof(T));
	}

	pointer address(reference value) const noexcept
	{
		std::addressof(value);
	}

	const_pointer address(const_reference value) const noexcept
	{	
		return std::addressof(value);
	}

public:
	__declspec(allocator) pointer allocate(size_type n)
	{
		// Attempt to allocate
		Blk blk = detail::AllocateIf<AllocatorType>::apply(m_Allocator, sizeof(T) * n);

		// Ensure memory was acquired
		if (!blk) 
			throw std::bad_alloc{ };

		// Store size in prefix object
		auto pPrefix = m_Allocator.Allocator().GetPrefixObject(blk);
		pPrefix->Size = blk.Size;

		return static_cast<pointer>(blk.Ptr);
	}

	__declspec(allocator) pointer allocate(size_type n, const void* /* pHint */)
	{
		return allocate(n);
	}

	void deallocate(pointer p, size_type n)
	{
		// The AffixAllocator doesn't need to know a block's size to calculate the 
		// prefix object from a pointer.  A temporary block will be used.
		Blk blk{ p, 1 };

		// Retrieve the block size
		auto pPrefix = m_Allocator.Allocator().GetPrefixObject(blk);
		blk.Size = pPrefix->Size;

		// Attempt to deallocate
		detail::DeallocateIf<AllocatorType>::apply(m_Allocator, blk);
	}

public:
	template<class U, class... Args>
	void construct(U* p, Args&&... args)
	{
		::new (static_cast<void*>(p)) U{ std::forward<Args>(args)... };
	}

	template<class U>
	void destroy(U* p)
	{
		p->~U();
	}
};

/// STLAllocatorImpl<void, A>
template<class A>
class Epic::detail::STLAllocatorImpl<void, A>
{
public:
	using type = Epic::detail::STLAllocatorImpl<void, A>;

public:
	using value_type = void;
	using pointer = void*;
	using const_pointer = const void*;

public:
	template<class U>
	struct rebind
	{
		using other = Epic::detail::STLAllocatorImpl<U, A>;
	};

public:
	STLAllocatorImpl() noexcept = default;
	STLAllocatorImpl(const type&) noexcept = default;

	template<class U>
	STLAllocatorImpl(const STLAllocatorImpl<U, A>&) noexcept { }

	template<class U>
	STLAllocatorImpl<void, A>& operator = (const STLAllocatorImpl<U, A>&)
	{
		return (*this);
	}
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T, class U, class A>
	inline bool operator == (const detail::STLAllocatorImpl<T, A>&, const detail::STLAllocatorImpl<U, A>&) noexcept
	{
		return true;
	}

	template<class T, class U, class Ta, class Ua>
	inline bool operator == (const detail::STLAllocatorImpl<T, Ta>&, const detail::STLAllocatorImpl<U, Ua>&) noexcept
	{
		return false;
	}

	template<class T, class U, class A>
	inline bool operator != (const detail::STLAllocatorImpl<T, A>&, const detail::STLAllocatorImpl<U, A>&) noexcept
	{
		return false;
	}

	template<class T, class U, class Ta, class Ua>
	inline bool operator != (const detail::STLAllocatorImpl<T, Ta>&, const detail::STLAllocatorImpl<U, Ua>&) noexcept
	{
		return true;
	}
}

//////////////////////////////////////////////////////////////////////////////

/// std::allocator_traits<T, A>
template<class T, class A>
struct std::allocator_traits<Epic::detail::STLAllocatorImpl<T, A>>
{
	using allocator_type = Epic::detail::STLAllocatorImpl<T, A>;

	using value_type = T;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;

	using void_pointer = void*;
	using const_void_pointer = const void*;
	
	using propagate_on_container_copy_assignment = std::true_type;
	using propagate_on_container_move_assignment = std::true_type;
	using propagate_on_container_swap = std::true_type;
	using is_always_equal = std::false_type;

	template<class U>
	using rebind_alloc = Epic::detail::STLAllocatorImpl<U, A>;

	template<class U>
	using rebind_traits = allocator_traits<Epic::detail::STLAllocatorImpl<U, A> >;

	static _declspec(allocator) pointer allocate(allocator_type& allocator, size_type n)
	{  return (allocator.allocate(n));  }

	static _declspec(allocator) pointer allocate(allocator_type& allocator, size_type n, const_void_pointer pHint)
	{  return (allocator.allocate(n, pHint));  }

	static void deallocate(allocator_type& allocator, pointer p, size_type n)
	{  (allocator.deallocate(p, n));  }

	template<class T, class... Args>
	static void construct(allocator_type& allocator, T* p, Args&&... args)
	{  (allocator.construct(p, std::forward<Args>(args)...));  }

	template<class T>
	static void destroy(allocator_type& allocator, T* p)
	{  (allocator.destroy(p));  }

	static size_type max_size(const allocator_type& allocator) noexcept
	{  return (allocator.max_size());  }

	static allocator_type select_on_container_copy_construction(const allocator_type& allocator)
	{  return (allocator);  }
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T, class Allocator>
	using STLAllocator = detail::STLAllocatorImpl<T, STLAllocatorAdapted<Allocator>>;
}
