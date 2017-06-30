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
		AudioAllocator
	};

	template<eConfigProperty P, class D, class C>
	struct ConfigProperty;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class T>
	using HasDefaultAllocator = typename T::DefaultAllocator;

	template<class T>
	using HasAudioAllocator = typename T::AudioAllocator;
}

//////////////////////////////////////////////////////////////////////////////

template<Epic::detail::eConfigProperty P, class D, class C>
struct Epic::detail::ConfigProperty
{
	using Type = D;
};

template<class D, class C>
struct Epic::detail::ConfigProperty<Epic::detail::eConfigProperty::DefaultAllocator, D, C>
{
	using Type = Epic::TMP::DetectedOrT<D, Epic::detail::HasDefaultAllocator, C>;
};

template<class D, class C>
struct Epic::detail::ConfigProperty<Epic::detail::eConfigProperty::AudioAllocator, D, C>
{
	using Type = Epic::TMP::DetectedOrT<D, Epic::detail::HasAudioAllocator, C>;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<eConfigProperty Prop, class C = Epic::Config<true>>
	using GetConfigProperty = ConfigProperty<Prop, Epic::TMP::detail::InvalidType, C>;

	template<eConfigProperty Prop, class Default, class C = Epic::Config<true>>
	using GetConfigPropertyOr = ConfigProperty<Prop, Default, C>;
}
