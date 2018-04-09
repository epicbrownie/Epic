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
		class GainImpl;
	}

	template<class Inner = Linear>
	struct Gain;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
class Epic::Math::XForm::detail::GainImpl
{
private:
	mutable detail::BiasImpl<T, detail::LinearImpl<T>> m_BiasFilter;

public:
	T Gain;
	Inner GainFilter;

public:
	inline T operator() (T t) const noexcept
	{
		const T tprime = GainFilter(t);
		const T thalf = T(0.5);
		const T tone = T(1);
		const T ttwo = T(2);

		m_BiasFilter.Bias = tone - Gain;

		return tprime < thalf
			? m_BiasFilter(ttwo * tprime) * thalf
			: tone - (m_BiasFilter(ttwo - (ttwo * tprime)) * thalf);
	}
};

template<class T>
class Epic::Math::XForm::detail::GainImpl<T, Epic::Math::XForm::detail::LinearImpl<T>>
{
private:
	mutable detail::BiasImpl<T, detail::LinearImpl<T>> m_BiasFilter;

public:
	T Gain;

public:
	inline T operator() (T t) const noexcept
	{
		const T thalf = T(0.5);
		const T tone = T(1);
		const T ttwo = T(2);

		m_BiasFilter.Bias = tone - Gain;

		return t < thalf
			? m_BiasFilter(ttwo * t) * thalf
			: tone - (m_BiasFilter(ttwo - (ttwo * t)) * thalf);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Gain
	: public detail::XFormImpl1<Inner, detail::GainImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Gain<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::GainImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Gain1 = Gain<>;
}
