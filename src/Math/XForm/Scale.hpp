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
		struct ScaleImpl;
	}

	template<class Inner = Linear>
	struct Scale;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::ScaleImpl
{
	Inner ScaleFilter;
	T Scale = (T)1;

	// NOTE: Equivalent to Modulate<Constant, Inner>
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = ScaleFilter(t);

		return Scale * tprime;
	}
};

template<class T>
struct Epic::Math::XForm::detail::ScaleImpl<T, Epic::Math::XForm::detail::LinearImpl<T>>
{
	T Scale = (T)1;

	// NOTE: Equivalent to Modulate<Constant, Linear>
	constexpr T operator() (T t) const noexcept
	{
		return Scale * t;
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Scale
	: public detail::XFormImpl1<Inner, detail::ScaleImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Scale<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::ScaleImpl> { };
