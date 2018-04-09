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
		struct FlipImpl;
	}

	template<class Inner = Linear>
	struct Flip;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::FlipImpl
{
	Inner FlipFilter;

	constexpr T operator() (T t) const noexcept
	{
		return (T)1 - FlipFilter(t);
	}
};

template<class T>
struct Epic::Math::XForm::detail::FlipImpl<T, Epic::Math::XForm::detail::LinearImpl<T>>
{
	constexpr T operator() (T t) const noexcept
	{
		return (T)1 - t;
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Flip
	: public detail::XFormImpl1<Inner, detail::FlipImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Flip<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::FlipImpl> { };
