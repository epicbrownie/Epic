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

#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class Allocator, class Tag>
	class GlobalAllocatorImpl;

	namespace detail
	{
		template<class T>
		struct IsGlobal;

		template<class A> 
		struct UnwrapGlobal;
	}
}

//////////////////////////////////////////////////////////////////////////////

/// IsGlobal<T>
template<class T>
struct Epic::detail::IsGlobal
{
private:
	template<class U> 
	static std::true_type HasAllocator(U*, decltype(T::Allocator())* = nullptr);
	static std::false_type HasAllocator(...);

public:
	typedef decltype(HasAllocator((T*)(nullptr))) Return;
	static constexpr bool value = Return::value;
};

//////////////////////////////////////////////////////////////////////////////

template<class A>
struct Epic::detail::UnwrapGlobal
{
	using Type = A;
};

template<class A, class Tag>
struct Epic::detail::UnwrapGlobal<Epic::GlobalAllocatorImpl<A, Tag>>
{
	using Type = typename UnwrapGlobal<A>::Type;
};
