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

#include <Epic/Memory/GlobalAllocator.hpp>
#include <Epic/Memory/AffixAllocator.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <Epic/Memory/detail/AllocatorHelpers.hpp>
#include <cassert>
#include <memory>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		template<class T, class A>
		class AllocI;

		template<class A, class Tag>
		struct AllocA;

		struct AllocPre;
	}

	template<class Allocator, class Tag = detail::GlobalAllocatorTag>
	struct AllocAdapted;
}

//////////////////////////////////////////////////////////////////////////////

/// AllocI<T, A>
template<class T, class A>
class Epic::detail::AllocI
{
	static_assert(!std::is_const<T>::value, "The C++ Standard forbids containers of const elements.");
	
public:
	using Type = Epic::detail::AllocI<T, A>;
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
		using other = Epic::detail::AllocI<U, A>;
	};

public:
	AllocI() noexcept { }

	AllocI(const Type& obj) noexcept { }

	template<class U>
	AllocI(const AllocI<U, A>&) noexcept
	{ /* m_Allocator should not be copied */ }

	template<class U>
	AllocI<T, A>& operator = (const AllocI<U, A>&) noexcept
	{
		/* m_Allocator should not be copied */
		return (*this);
	}

public:
	constexpr size_t max_size() const noexcept
	{
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
		Blk blk;

		constexpr bool IsAligned = !detail::CanAllocate<AllocatorType>::value ||
								   (AllocatorType::Alignment % AlignOf<T>::value) != 0;

		if constexpr (IsAligned)
		{
			static_assert(detail::CanAllocateAligned<AllocatorType>::value,
				"STLAllocator::Allocate() - This type requires an allocator that is capable of "
				"performing arbitrarily aligned allocations");

			// Attempt to allocate aligned memory via AllocateAligned()
			blk = m_Allocator.AllocateAligned(sizeof(T) * n, AlignOf<T>::value);

			// Ensure memory was acquired
			if (!blk) throw std::bad_alloc{};

			// Store size in prefix object
			auto pPrefix = m_Allocator.Allocator().GetPrefixObject(blk, AlignOf<T>::value);
			pPrefix->Size = blk.Size;
		}
		else
		{
			static_assert(detail::CanAllocate<AllocatorType>::value,
				"STLAllocator::Allocate() - This type requires an allocator that is capable of "
				"performing unaligned allocations");

			// Attempt to allocate memory via Allocate()
			blk = m_Allocator.Allocate(sizeof(T) * n);

			// Ensure memory was acquired
			if (!blk) throw std::bad_alloc{};

			// Store size in prefix object
			auto pPrefix = m_Allocator.Allocator().GetPrefixObject(blk);
			pPrefix->Size = blk.Size;
		}

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

		constexpr bool IsAligned = !detail::CanAllocate<AllocatorType>::value ||
								   (AllocatorType::Alignment % AlignOf<T>::value) != 0;

		if constexpr (IsAligned)
		{
			// AllocateAligned was used
			const auto pPrefix = m_Allocator.Allocator().GetPrefixObject(blk, std::alignment_of_v<T>);
			blk.Size = pPrefix->Size;

			if constexpr (detail::CanDeallocateAligned<AllocatorType>::value)
				m_Allocator.DeallocateAligned(blk);
		}
		else
		{
			// Allocate was used
			const auto pPrefix = m_Allocator.Allocator().GetPrefixObject(blk);
			blk.Size = pPrefix->Size;

			if constexpr (detail::CanDeallocate<AllocatorType>::value)
				m_Allocator.Deallocate(blk);
		}
	}

public:
	template<class U, class... Args>
	void construct(U* p, Args&&... args)
	{
		::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
	}

	template<class U>
	void destroy(U* p)
	{
		p->~U();
	}

public:
	Type select_on_container_copy_construction() const
	{
		return Type();
	}
};

/// AllocI<void, A>
template<class A>
class Epic::detail::AllocI<void, A>
{
public:
	using Type = Epic::detail::AllocI<void, A>;

public:
	using value_type = void;
	using pointer = void*;
	using const_pointer = const void*;

public:
	template<class U>
	struct rebind
	{
		using other = Epic::detail::AllocI<U, A>;
	};

public:
	AllocI() noexcept = default;
	AllocI(const Type&) noexcept = default;

	template<class U>
	AllocI(const AllocI<U, A>&) noexcept { }

	template<class U>
	AllocI<void, A>& operator = (const AllocI<U, A>&)
	{
		return (*this);
	}
};

//////////////////////////////////////////////////////////////////////////////

// Comparison operators
namespace Epic
{
	template<class T, class U, class A>
	constexpr bool operator == (const detail::AllocI<T, A>&, const detail::AllocI<U, A>&) noexcept
	{
		return true;
	}

	template<class T, class U, class Ta, class Ua>
	constexpr bool operator == (const detail::AllocI<T, Ta>&, const detail::AllocI<U, Ua>&) noexcept
	{
		return false;
	}

	template<class T, class U, class A>
	constexpr bool operator != (const detail::AllocI<T, A>&, const detail::AllocI<U, A>&) noexcept
	{
		return false;
	}

	template<class T, class U, class Ta, class Ua>
	constexpr bool operator != (const detail::AllocI<T, Ta>&, const detail::AllocI<U, Ua>&) noexcept
	{
		return true;
	}
}

//////////////////////////////////////////////////////////////////////////////

/// std::allocator_traits<T, A>
template<class T, class A>
struct std::allocator_traits<Epic::detail::AllocI<T, A>>
{
	using allocator_type = Epic::detail::AllocI<T, A>;

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
	using rebind_alloc = Epic::detail::AllocI<U, A>;

	template<class U>
	using rebind_traits = allocator_traits<Epic::detail::AllocI<U, A> >;

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
	{  return (allocator.select_on_container_copy_construction());  }
};

//////////////////////////////////////////////////////////////////////////////

struct Epic::detail::AllocPre
{
	Epic::MemoryBlock::SizeType Size;
};

//////////////////////////////////////////////////////////////////////////////

template<class A, class Tag>
struct Epic::detail::AllocA
{
	using _affixed = Epic::AffixAllocator<A, Epic::detail::AllocPre>;

	using Type = Epic::GlobalAllocator<_affixed, Tag>;
};

template<class A, class Tag, class OldTag>
struct Epic::detail::AllocA<Epic::detail::GlobalAllocatorImpl<A, OldTag>, Tag>
{
	using _unwrapped = typename detail::UnwrapGlobal<A>::Type;
	using _affixed = Epic::AffixAllocator<_unwrapped, Epic::detail::AllocPre>;

	using Type = Epic::GlobalAllocator<_affixed, OldTag>;
};

template<class A, class Tag, class T>
struct Epic::detail::AllocA<Epic::detail::AllocI<T, A>, Tag>
{
	using Type = Epic::GlobalAllocator<typename A::AllocatorType, typename A::Tag>;
};

//////////////////////////////////////////////////////////////////////////////

template<class A, class Tag>
struct Epic::AllocAdapted : public Epic::detail::AllocA<A, Tag>::Type
{
	using Base = typename Epic::detail::AllocA<A, Tag>::Type;

	using Base::Base;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		using STLAllocatorPrefix = AllocPre;

		template<class A, class Tag>
		using STLAllocatorAdaptor = AllocA<A, Tag>;

		template<class T, class A>
		using STLAllocatorImpl = AllocI<T, A>;
	}

	template<class A, class Tag = Epic::detail::GlobalAllocatorTag>
	using STLAllocatorAdapted = AllocAdapted<A, Tag>;

	template<class T, class Allocator, class Tag = Epic::detail::GlobalAllocatorTag>
	using STLAllocator = detail::STLAllocatorImpl<T, STLAllocatorAdapted<Allocator, Tag>>;
}
