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

#include <Epic/STL/Default.hpp>
#include <Epic/STL/Allocator.hpp>
#include <memory>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class T, class A>
	struct MakeSharedPtrSTLAllocator;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class A>
struct Epic::detail::MakeSharedPtrSTLAllocator
{
	using Type = Epic::STLAllocator<T, A>;
};

template<class T, class A>
struct Epic::detail::MakeSharedPtrSTLAllocator<T, Epic::detail::STLAllocatorImpl<T, A>>
{
	using Type = A;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// MakeShared<T, A, Args...>
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::SharedPtr>, class... Args>
	inline std::shared_ptr<T> MakeShared(Args&&... args)
	{
		using AdaptedAllocator = typename detail::MakeSharedPtrSTLAllocator<T, A>::Type;
		AdaptedAllocator allocator;

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

		// Create a Deleter
		auto Deleter = [=] (auto p) -> void
		{
			AdaptedAllocator alloc;
			alloc.destroy(p); 
			alloc.deallocate(p, 1); 
		};

		return std::shared_ptr<T>{ pObject, Deleter, allocator };
	}
}
