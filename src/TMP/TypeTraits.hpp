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
#include <utility>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
	/*	MSVC 15.2 has an issue with decltype bug with std::void_t.
		The following is a temporary workaround until 15.3 is released. */

	namespace detail
	{
		template<class...>
		struct Voided
		{
			using Type = void;
		};

		template<class... Args>
		using VoidedT = typename Voided<Args...>::Type;
	}

	/*	An implementation of the 'Detection Idiom' as seen in Marshall Clow's talk:
		C++Now 2017: Marshall Clow "The 'Detection Idiom:' A Better Way to SFINAE" */

	namespace detail
	{
		struct InvalidType
		{
			InvalidType() = delete;
			~InvalidType() = delete;
			InvalidType(const InvalidType&) = delete;
			void operator = (const InvalidType&) = delete;
		};

		template<class Default, class Void, template<class...> class Op, class... Args>
		struct Detector : std::false_type
		{
			using ValueType = std::false_type;
			using Type = Default;
		};

		template<class Default, template<class...> class Op, class... Args>
		struct Detector<Default, VoidedT<Op<Args...>>, Op, Args...>
		{
			using ValueType = std::true_type;
			using Type = Op<Args...>;
		};
	}

	template<template<class...> class Op, class... Args>
	using IsDetected = typename detail::Detector<detail::InvalidType, void, Op, Args...>::ValueType;

	template<template<class...> class Op, class... Args>
	using DetectedT = typename detail::Detector<detail::InvalidType, void, Op, Args...>::Type;

	template<class Default, template<class...> class Op, class... Args>
	using DetectedOrT = typename detail::Detector<Default, void, Op, Args...>::Type;
	
	template<class Expected, template<class...> class Op, class... Args>
	using IsDetectedExact = std::is_same<Expected, DetectedT<Op, Args...>>;

	template<class To, template<class...> class Op, class... Args>
	using IsDetectedConvertible = std::is_convertible<DetectedT<Op, Args...>, To>;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
	/*	Derives from std::true_type if Function(Args...) could be called and would result in
		a type that is implicitly convertible to ReturnType.  Derives from std::false_type otherwise. */

	template<class Function, class Return, class Enable = void>
	struct IsCallable : std::false_type { };

	template<class Function, class Return, class... Ts>
	struct IsCallable<Function(Ts...), Return,
		std::void_t< decltype(std::declval<Function>() (std::declval<Ts>()...))> >
		: std::is_convertible<decltype(std::declval<Function>() (std::declval<Ts>()...)), Return>
	{ };
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
	/*	Derives from std::true_type if type U is explicitly convertible to T.
		That is, if T(U&) is valid, but an implicit conversion to T from U is not.
		Derives from std::false_type otherwise. */

	template <class U, class T>
	struct IsExplicitlyConvertible :
		std::conditional_t<std::is_constructible<T, U>::value && !std::is_convertible<U, T>::value, 
						   std::true_type, std::false_type>
	{ };
}
