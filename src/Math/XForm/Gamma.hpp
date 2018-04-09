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
#include <cmath>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, class Inner>
		struct GammaImpl;
	}

	template<class Inner = Linear>
	struct Gamma;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::GammaImpl
{
	T Gamma;
	Inner GammaFilter;

	constexpr T operator() (T t) const noexcept
	{
		const T tprime = GammaFilter(t);

		return std::pow(tprime, (T)1 / Gamma);
	}
};

template<class T>
struct Epic::Math::XForm::detail::GammaImpl<T, Epic::Math::XForm::detail::LinearImpl<T>>
{
	T Gamma;

	constexpr T operator() (T t) const noexcept
	{
		return std::pow(t, T(1) / Gamma);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Gamma
	: public detail::XFormImpl1<Inner, detail::GammaImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Gamma<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::GammaImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Gamma1 = Gamma<>;
}
