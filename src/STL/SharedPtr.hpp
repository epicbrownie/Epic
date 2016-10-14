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
#include <Epic/STL/UniquePtr.hpp>
#include <memory>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// MakeShared<T, A, Args...>
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::SharedPtr>, class... Args>
	inline std::shared_ptr<T> MakeShared(Args&&... args)
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

		// Create a Deleter
		auto Deleter = [=] (auto p) -> void
		{
			AllocatorType alloc;
			AllocatorTraits::destroy(alloc, p); 
			AllocatorTraits::deallocate(alloc, p, 1); 
		};

		return std::shared_ptr<T>{ pObject, Deleter, allocator };
	}

	/// MakeShared<std::unique_ptr<T, D>>
	template<class T, class D>
	inline std::shared_ptr<T> MakeShared(std::unique_ptr<T, D>&& pUnique)
	{
		return std::shared_ptr<T>{ std::move(pUnique) };
	}
}
