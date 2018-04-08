//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2018 Ronnie Brohn (EpicBrownie)      
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

#include <Epic/Math/XForm/detail/Implementation.hpp>
#include <Epic/Math/XForm/Linear.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, class Inner>
		struct MapImpl;
	}

	template<class Inner = Linear>
	struct Map;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::MapImpl
{
	Inner MapFilter;
	T InMin = T(0);
	T InMax = T(1);
	T OutMin = T(0);
	T OutMax = T(1);

	constexpr T operator() (T t) const noexcept
	{
		const T inRange = InMax - InMin;
		const T outRange = OutMax - OutMin;

		return (MapFilter((t - InMin) / inRange) * outRange) + OutMin;
	}
};

template<class T>
struct Epic::Math::XForm::detail::MapImpl<T, Epic::Math::XForm::detail::LinearImpl<T>>
{
	T InMin = T(0);
	T InMax = T(1);
	T OutMin = T(0);
	T OutMax = T(1);

	constexpr T operator() (T t) const noexcept
	{
		const T inRange = InMax - InMin;
		const T outRange = OutMax - OutMin;

		return ((t - InMin) / inRange * outRange) + OutMin;
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Map
	: public detail::XFormImpl1<Inner, detail::MapImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Map<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::MapImpl> { };
