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
		struct BiasImpl;
	}

	template<class Inner = Linear>
	struct Bias;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::BiasImpl
{
	T Bias;
	Inner BiasFilter;

	constexpr T operator() (T t) const noexcept
	{
		const T tprime = BiasFilter(t);
		const T exp = T(std::log(Bias) / std::log(T(0.5)));

		return std::pow(tprime, exp);
	}
};

template<class T>
struct Epic::Math::XForm::detail::BiasImpl<T, Epic::Math::XForm::detail::LinearImpl<T>>
{
	T Bias;

	constexpr T operator() (T t) const noexcept
	{
		const T exp = T(std::log(Bias) / std::log(T(0.5)));

		return std::pow(t, exp);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Bias
	: public detail::XFormImpl1<Inner, detail::BiasImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Bias<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::BiasImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Bias1 = Bias<>;
}
