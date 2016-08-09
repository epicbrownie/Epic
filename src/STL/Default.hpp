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
#include <Epic/Memory/NedAllocator.hpp>
#include <Epic/Memory/AlignedNedAllocator.hpp>
#include <Epic/Memory/AlignmentAllocator.hpp>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	enum class eAllocatorFor
	{
		String, WString,

		List,
		ForwardList,
		Deque,
		Vector,
		Map,
		MultiMap,
		UnorderedMap,
		Set,
		MultiSet,
		UnorderedSet,

		StringStream, IStringStream, OStringStream,
		WStringStream, WIStringStream, WOStringStream,

		UniquePtr, SharedPtr
	};
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class T, Epic::eAllocatorFor DefaultForType>
	struct HasClassDefaultAllocator;

	template<class T, Epic::eAllocatorFor DefaultForType, bool Enabled>
	struct GetClassDefaultAllocator;
}

namespace Epic
{
	template<class T, Epic::eAllocatorFor DefaultForType>
	struct DefaultAllocator;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		using DefaultAllocatorType = std::conditional_t<
				std::is_same<void, typename GetConfigProperty<eConfigProperty::DefaultAllocator>::Type>::value,
				AlignmentAllocator<AlignedNedAllocator, NedAllocator>,
				typename GetConfigProperty<eConfigProperty::DefaultAllocator>::Type
			  >;
	}
}

//////////////////////////////////////////////////////////////////////////////

template<class T, Epic::eAllocatorFor DefaultForType>
struct Epic::detail::HasClassDefaultAllocator
{
	static constexpr bool value = false;
};

template<class T>
struct Epic::detail::HasClassDefaultAllocator<T, Epic::eAllocatorFor::UniquePtr>
{
private:
	template<class U>
	static std::true_type HasDefault(U*, decltype(T::DefaultAllocator())* = nullptr);
	static std::false_type HasDefault(...);

	using Return = decltype(HasDefault((T*)(nullptr)));

public:
	static constexpr bool value = Return::value;
};

template<class T>
struct Epic::detail::HasClassDefaultAllocator<T, Epic::eAllocatorFor::SharedPtr>
{
private:
	template<class U>
	static std::true_type HasDefault(U*, decltype(T::DefaultAllocator())* = nullptr);
	static std::false_type HasDefault(...);

	using Return = decltype(HasDefault((T*)(nullptr)));

public:
	static constexpr bool value = Return::value;
};


template<class T, Epic::eAllocatorFor ClassDefaultForType, bool Enabled>
struct Epic::detail::GetClassDefaultAllocator
{
	using Type = detail::DefaultAllocatorType;
};

template<class T>
struct Epic::detail::GetClassDefaultAllocator<T, Epic::eAllocatorFor::UniquePtr, true>
{
	using Type = typename T::DefaultAllocator;
};

template<class T>
struct Epic::detail::GetClassDefaultAllocator<T, Epic::eAllocatorFor::SharedPtr, true>
{
	using Type = typename T::DefaultAllocator;
};


namespace Epic
{
	namespace detail
	{
		template<class T, Epic::eAllocatorFor ClassDefaultForType>
		using ClassDefaultAllocator = typename GetClassDefaultAllocator<T, ClassDefaultForType, 
											   HasClassDefaultAllocator<T, ClassDefaultForType>::value>::Type;
	}
}

//////////////////////////////////////////////////////////////////////////////

template<class T, Epic::eAllocatorFor DefaultForType>
struct Epic::DefaultAllocator
{
	using Type = detail::DefaultAllocatorType;
};

template<class T>
struct Epic::DefaultAllocator<T, Epic::eAllocatorFor::UniquePtr>
{
	/*
		std::unique_ptr<T> for all T's cannot be specialized.
		Either specialize on a specific T, or add a DefaultAllocator typedef to the T class.
	*/
	
	using U = std::remove_pointer_t<std::decay_t<T>>;
	using Type = detail::ClassDefaultAllocator<U, Epic::eAllocatorFor::UniquePtr>;
};

template<class T>
struct Epic::DefaultAllocator<T[], Epic::eAllocatorFor::UniquePtr>
{
	/*
	std::unique_ptr<T[]> for all T's cannot be specialized.
	Either specialize on a specific T[], or add a DefaultAllocator typedef to the T class.
	*/

	using U = std::remove_pointer_t<std::decay_t<T>>;
	using Type = detail::ClassDefaultAllocator<U, Epic::eAllocatorFor::UniquePtr>;
};

template<class T>
struct Epic::DefaultAllocator<T, Epic::eAllocatorFor::SharedPtr>
{
	/*
		std::shared_ptr<T> for all T's cannot be specialized.
		Either specialize on a specific T, or add a DefaultAllocator typedef to the T class.
	*/

	using U = std::remove_pointer_t<std::decay_t<T>>;
	using Type = detail::ClassDefaultAllocator<U, Epic::eAllocatorFor::SharedPtr>;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T, eAllocatorFor DefaultForType>
	using DefaultAllocatorFor = typename Epic::DefaultAllocator<T, DefaultForType>::Type;
}
