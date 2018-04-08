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
#include <Epic/Math/XForm/Angle.hpp>
#include <Epic/Math/Angle.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, class Inner>
		struct SineImpl;
	}

	template<class Inner = Angle<>>
	struct Sine;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::SineImpl
{
	Inner SinFilter;

	constexpr T operator() (T t) const noexcept
	{
		const T tprime = SinFilter(t);

		return std::sin(tprime);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Sine
	: public detail::XFormImpl1<Inner, detail::SineImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Sine<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::SineImpl> { };
