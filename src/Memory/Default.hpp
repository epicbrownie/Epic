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

#include <Epic/detail/ReadConfig.hpp>
#include <Epic/Memory/Mallocator.hpp>
#include <Epic/TMP/TypeTraits.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	enum class eAllocatorFor
	{
		New, UniquePtr, SharedPtr,

		String,

		List,
		ForwardList,
		Deque,
		Vector,
		Map,
		Set,

		StringStream, IStringStream, OStringStream,

		TemporaryBuffer, StorageBuffer
	};
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class T, Epic::eAllocatorFor DefaultForType>
	struct ClassDefaultAllocator;

	template<class T>
	using HasClassDefaultAllocator = typename T::DefaultAllocator;
}

namespace Epic
{
	template<class T, Epic::eAllocatorFor DefaultForType>
	struct DefaultAllocator;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	/// The configured default allocator type for the entire system.
	using DefaultAllocatorType = typename detail::GetConfigPropertyOr<detail::eConfigProperty::DefaultAllocator, Mallocator>::type;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, Epic::eAllocatorFor DefaultForType>
struct Epic::detail::ClassDefaultAllocator
{
	using type = DefaultAllocatorType;
};

template<class T>
struct Epic::detail::ClassDefaultAllocator<T, Epic::eAllocatorFor::New>
{
	using type = Epic::TMP::DetectedOrT<DefaultAllocatorType, HasClassDefaultAllocator, T>;
};

template<class T>
struct Epic::detail::ClassDefaultAllocator<T, Epic::eAllocatorFor::UniquePtr>
{
	using type = Epic::TMP::DetectedOrT<DefaultAllocatorType, HasClassDefaultAllocator, T>;
};

template<class T>
struct Epic::detail::ClassDefaultAllocator<T, Epic::eAllocatorFor::SharedPtr>
{
	using type = Epic::TMP::DetectedOrT<DefaultAllocatorType, HasClassDefaultAllocator, T>;
};

//////////////////////////////////////////////////////////////////////////////

template<class T, Epic::eAllocatorFor DefaultForType>
struct Epic::DefaultAllocator
{
	using type = DefaultAllocatorType;
};

template<class T>
struct Epic::DefaultAllocator<T, Epic::eAllocatorFor::New>
{
	/*
		operator new for all T's cannot be specialized.
		Either specialize on a specific T, or add a DefaultAllocator typedef to the T class.
	*/

	using U = std::remove_pointer_t<std::decay_t<T>>;
	using type = typename detail::ClassDefaultAllocator<U, Epic::eAllocatorFor::New>::type;
};

template<class T>
struct Epic::DefaultAllocator<T[], Epic::eAllocatorFor::New>
{
	/*
		operator new[] for all T's cannot be specialized.
		Either specialize on a specific T[], or add a DefaultAllocator typedef to the T class.
	*/

	using U = std::remove_pointer_t<std::decay_t<T>>;
	using type = typename detail::ClassDefaultAllocator<U, Epic::eAllocatorFor::New>::type;
};

template<class T>
struct Epic::DefaultAllocator<T, Epic::eAllocatorFor::UniquePtr>
{
	/*
		std::unique_ptr<T> for all T's cannot be specialized.
		Either specialize on a specific T, or add a DefaultAllocator typedef to the T class.
	*/
	
	using U = std::remove_pointer_t<std::decay_t<T>>;
	using type = typename detail::ClassDefaultAllocator<U, Epic::eAllocatorFor::UniquePtr>::type;
};

template<class T>
struct Epic::DefaultAllocator<T[], Epic::eAllocatorFor::UniquePtr>
{
	/*
		std::unique_ptr<T[]> for all T's cannot be specialized.
		Either specialize on a specific T[], or add a DefaultAllocator typedef to the T class.
	*/

	using U = std::remove_pointer_t<std::decay_t<T>>;
	using type = typename detail::ClassDefaultAllocator<U, Epic::eAllocatorFor::UniquePtr>::type;
};

template<class T>
struct Epic::DefaultAllocator<T, Epic::eAllocatorFor::SharedPtr>
{
	/*
		std::shared_ptr<T> for all T's cannot be specialized.
		Either specialize on a specific T, or add a DefaultAllocator typedef to the T class.
	*/

	using U = std::remove_pointer_t<std::decay_t<T>>;
	using type = typename detail::ClassDefaultAllocator<U, Epic::eAllocatorFor::SharedPtr>::type;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T, eAllocatorFor DefaultForType>
	using DefaultAllocatorFor = typename Epic::DefaultAllocator<T, DefaultForType>::type;
}
