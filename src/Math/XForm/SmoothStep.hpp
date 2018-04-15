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
		struct SmoothStepImpl;
	}

	template<class Inner = Linear>
	struct SmoothStep;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::SmoothStepImpl
{
	Inner SmoothInner;

	constexpr T operator() (T t) const noexcept
	{
		const T tprime = SmoothInner(t);

		return tprime * tprime * (T(3) - (T(2) * tprime));
	}
};

template<class T>
struct Epic::Math::XForm::detail::SmoothStepImpl<T, Epic::Math::XForm::detail::LinearImpl<T>>
{
	constexpr T operator() (T t) const noexcept
	{
		return (t * t * (T(3) - (T(2) * t)));
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::SmoothStep
	: public detail::XFormImpl1<Inner, detail::SmoothStepImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::SmoothStep<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::SmoothStepImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using SmoothStep1 = SmoothStep<>;
}
