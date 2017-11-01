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

// Detection Idiom
namespace Epic::TMP
{
	/*	MSVC 15.2 has an issue with decltype bug with std::void_t.
		The following is a temporary workaround until 15.3 is released.
		UPDATE: Implementation switched back to std::void_t */

	namespace detail
	{
		template<class...>
		struct Voided
		{
			using type = void;
		};

		template<class... Args>
		using VoidedT = typename Voided<Args...>::type;
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
			using value_type = std::false_type;
			using type = Default;
		};

		template<class Default, template<class...> class Op, class... Args>
		struct Detector<Default, std::void_t<Op<Args...>>, Op, Args...>
		{
			using value_type = std::true_type;
			using type = Op<Args...>;
		};
	}

	template<template<class...> class Op, class... Args>
	using IsDetected = typename detail::Detector<detail::InvalidType, void, Op, Args...>::value_type;

	template<template<class...> class Op, class... Args>
	using DetectedT = typename detail::Detector<detail::InvalidType, void, Op, Args...>::type;

	template<class Default, template<class...> class Op, class... Args>
	using DetectedOrT = typename detail::Detector<Default, void, Op, Args...>::type;
	
	template<class Expected, template<class...> class Op, class... Args>
	using IsDetectedExact = std::is_same<Expected, DetectedT<Op, Args...>>;

	template<class To, template<class...> class Op, class... Args>
	using IsDetectedConvertible = std::is_convertible<DetectedT<Op, Args...>, To>;
}

//////////////////////////////////////////////////////////////////////////////

// IsCallable
namespace Epic::TMP
{
	/*	Derives from std::true_type if Function(Args...) could be called and would result in
		a type that is implicitly convertible to ReturnType.  Derives from std::false_type otherwise. */

//	template<class Function, class Return, class Enable = void>
//	struct IsCallable : std::false_type { };

//	template<class Function, class Return, class... Ts>
//	struct IsCallable<Function(Ts...), Return,
//		std::void_t< decltype(std::declval<Function>() (std::declval<Ts>()...))> >
//		: std::is_convertible<decltype(std::declval<Function>() (std::declval<Ts>()...)), Return>
//	{ };
}

//////////////////////////////////////////////////////////////////////////////

// IsExplicitlyConvertible
namespace Epic::TMP
{
	/*	Value is true if From type is explicitly convertible to To type.
		That is, if To(From&) is valid, but an implicit conversion to To from From is not. */

	template<class From, class To>
	struct IsExplicitlyConvertible
	{
		static constexpr bool Value = std::is_constructible_v<To, From> && !std::is_convertible_v<From, To>;
	};

	template<class From, class To>
	static constexpr bool IsExplicitlyConvertibleV = IsExplicitlyConvertible<From, To>::Value;
}

//////////////////////////////////////////////////////////////////////////////

// Container Traits
namespace Epic::TMP
{
	namespace detail
	{
		template<class C, class... Args>
		using CanEmplaceImpl = decltype(std::declval<C>().emplace(std::declval<Args>()...));
		
		template<class C, class... Args>
		using CanEmplaceBackImpl = decltype(std::declval<C>().emplace_back(std::declval<Args>()...));

		template<class C>
		using CanCStrImpl = decltype(std::declval<const C>().c_str());

		template<class C, class I>
		using IsIndexableImpl = decltype(std::declval<C>()[std::declval<I>()] = std::declval<typename C::value_type>());

		template<class C>
		using HasKeyMemberImpl = typename C::key_type;

		template<class C>
		using HasValueMemberImpl = typename C::value_type;

		template<class C>
		using HasMappedMemberImpl = typename C::mapped_type;

		template<class C>
		using HasTraitsMemberImpl = typename C::traits_type;
	}

	template<class C>
	constexpr bool HasKeyMember = IsDetected<detail::HasKeyMemberImpl, C>::value;

	template<class C>
	constexpr bool HasValueMember = IsDetected<detail::HasValueMemberImpl, C>::value;

	template<class C>
	constexpr bool HasMappedMember = IsDetected<detail::HasMappedMemberImpl, C>::value;

	template<class C>
	constexpr bool HasTraitsMember = IsDetected<detail::HasTraitsMemberImpl, C>::value;

	template<class C, bool Enabled = HasValueMember<C>>
	struct IsVectorLike : std::false_type { };

	template<class C>
	struct IsVectorLike<C, true>
	{
		static constexpr bool value = IsDetected<detail::CanEmplaceBackImpl, C, typename C::value_type>::value;
	};

	template<class C, bool Enabled = HasKeyMember<C> && HasValueMember<C> && !HasMappedMember<C>>
	struct IsSetLike : std::false_type { };

	template<class C>
	struct IsSetLike<C, true>
	{
		static constexpr bool value = IsDetected<detail::CanEmplaceImpl, C, typename C::key_type>::value;
	};

	template<class C, bool Enabled = HasKeyMember<C> && HasMappedMember<C>>
	struct IsMapLike : std::false_type { };

	template<class C>
	struct IsMapLike<C, true>
	{
		static constexpr bool value = IsDetected<detail::CanEmplaceImpl, C, typename C::key_type, typename C::mapped_type>::value;
	};

	template<class C, bool Enabled = HasTraitsMember<C> && HasValueMember<C>>
	struct IsStringLike : std::false_type { };

	template<class C>
	struct IsStringLike<C, true>
	{
		static constexpr bool value = 
			std::is_same_v<C::traits_type::char_type, C::value_type> && IsDetected<detail::CanCStrImpl, C>::value;
	};

	template<class C, class I = size_t>
	struct IsIndexable
	{
		static constexpr bool value =
			std::disjunction_v<
				std::is_array<C>,
				std::conjunction<
					IsDetected<detail::HasValueMemberImpl, C>,
					IsDetected<detail::IsIndexableImpl, C, I>>>;
	};
}
