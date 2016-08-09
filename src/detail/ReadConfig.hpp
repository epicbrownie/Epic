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
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	enum class eConfigProperty
	{
		DefaultAllocator
	};
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<typename T, typename = void>
	struct CheckDefaultAllocator;

	template<typename T>
	struct ConfigProperties;

	template<Epic::detail::eConfigProperty Property, class Config = Epic::Config<true>>
	struct GetConfigProperty;
}

//////////////////////////////////////////////////////////////////////////////

/// CheckDefaultAllocator
template<typename T, typename>
struct Epic::detail::CheckDefaultAllocator
{
	/* DefaultAllocator not found in Epic::Config<true> */
	using DefaultAllocator = void;
};

template<typename T>
struct Epic::detail::CheckDefaultAllocator<T, std::void_t<typename T::DefaultAllocator>> 
{
	/* Import DefaultAllocator from Epic::Config<true> */
	using DefaultAllocator = typename T::DefaultAllocator;
};

//////////////////////////////////////////////////////////////////////////////

/// ConfigProperties
template<typename T>
struct Epic::detail::ConfigProperties :
	CheckDefaultAllocator<T>
{ 
	/* Properties imported through inheritance */
};

//////////////////////////////////////////////////////////////////////////////

/// GetConfigProperty<DefaultAllocator>
template<class ConfigType>
struct Epic::detail::GetConfigProperty<Epic::detail::eConfigProperty::DefaultAllocator, ConfigType>
{
	//static_assert(std::is_same<ConfigType, Epic::Config<true>>::value &&
	//			  !std::is_same<void, typename Epic::detail::ConfigProperties<ConfigType>::DefaultAllocator>::value,
	//	"You must specialize structure Epic::Config and add a DefaultAllocator member. "
	//	"See (https://github.com/epicbrownie/Epic/wiki) for details.");

	using Type = typename Epic::detail::ConfigProperties<ConfigType>::DefaultAllocator;
};
