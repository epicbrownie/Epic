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

#include <Epic/Config.hpp>
#include <Epic/TMP/TypeTraits.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	enum class eConfigProperty
	{
		DefaultAllocator,
		AudioAllocator,
	};

	enum class eConfigFactory
	{
		DynamicFilterPtr
	};

	template<eConfigProperty P, class D, class C>
	struct ConfigProperty;

	template<eConfigFactory F, template<class...> class D, class C, class... Args>
	struct ConfigFactory;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class T>
	using HasDefaultAllocator = typename T::DefaultAllocator;

	template<class T>
	using HasAudioAllocator = typename T::AudioAllocator;

	template<class T>
	using HasDynamicFilterPtr = typename T::DynamicFilterPtr;
}

//////////////////////////////////////////////////////////////////////////////

template<class D, class C>
struct Epic::detail::ConfigProperty<Epic::detail::eConfigProperty::DefaultAllocator, D, C>
{
	using type = Epic::TMP::DetectedOrT<D, Epic::detail::HasDefaultAllocator, C>;
};

template<class D, class C>
struct Epic::detail::ConfigProperty<Epic::detail::eConfigProperty::AudioAllocator, D, C>
{
	using type = Epic::TMP::DetectedOrT<D, Epic::detail::HasAudioAllocator, C>;
};

//////////////////////////////////////////////////////////////////////////////

template<template<class...> class D, class C, class... Args>
struct Epic::detail::ConfigFactory<Epic::detail::eConfigFactory::DynamicFilterPtr, D, C, Args...>
{
	struct DefaultFactory { template<class...> using type = D<Args...>; };

	template<template<class...> class F>
	using Factory = F<Args...>;

	using type = Factory<typename Epic::TMP::DetectedOrT<DefaultFactory, HasDynamicFilterPtr, C>::type>;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<eConfigProperty Prop>
	using GetConfigProperty = ConfigProperty<Prop, Epic::TMP::detail::InvalidType, Epic::Config<true>>;

	template<eConfigProperty Prop, class Default>
	using GetConfigPropertyOr = ConfigProperty<Prop, Default, Epic::Config<true>>;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<eConfigFactory Factory, class... Args>
	using GetConfigFactory = ConfigFactory<Factory, Epic::TMP::detail::InvalidTType, Epic::Config<true>, Args...>;

	template<eConfigFactory Factory, template<class...> class Default, class... Args>
	using GetConfigFactoryOr = ConfigFactory<Factory, Default, Epic::Config<true>, Args...>;
}
