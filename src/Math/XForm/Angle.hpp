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
#include <Epic/Math/Constants.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, class Inner>
		struct AngleImpl;
	}

	template<class Inner = Linear>
	struct Angle;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::AngleImpl
{
	Inner AngleFilter;

	// NOTE: Equivalent to Modulate<Constant, Inner>
	constexpr T operator() (T t) const noexcept
	{
		return Pi<T> * AngleFilter(t);
	}
};

template<class T>
struct Epic::Math::XForm::detail::AngleImpl<T, Epic::Math::XForm::detail::LinearImpl<T>>
{
	// NOTE: Equivalent to Modulate<Constant, Linear>
	constexpr T operator() (T t) const noexcept
	{
		return Pi<T> * t;
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Angle
	: public detail::XFormImpl1<Inner, detail::AngleImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Angle<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::AngleImpl> { };
