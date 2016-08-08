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
	struct MakeSTLAllocator;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class A>
struct Epic::detail::MakeSTLAllocator
{
	using Type = Epic::STLAllocator<T, A>;
};

template<class T, class A>
struct Epic::detail::MakeSTLAllocator<T, Epic::detail::STLAllocatorImpl<T, A>>
{
	using Type = A;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// MakeShared<T, A, Args...>
	template<class T, class A = Epic::DefaultAllocatorFor<T, Epic::eAllocatorFor::STLSharedPtr>, class... Args>
	std::shared_ptr<T> MakeShared(Args&&... args)
	{
		using AdaptedAllocator = typename detail::MakeSTLAllocator<T, A>::Type;
		AdaptedAllocator allocator;

		return std::allocate_shared<T, AdaptedAllocator>(allocator, std::forward<Args>(args)...);
	}
}
