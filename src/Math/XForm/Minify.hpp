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
		struct MinifyImpl;
	}

	template<class Inner = Linear>
	struct Minify;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::MinifyImpl
{
	Inner MinFilter;

	// NOTE: Equivalent to Modulate<Linear, Inner>
	constexpr T operator() (T t) const noexcept
	{
		return t * MinFilter(t);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Minify
	: public detail::XFormImpl1<Inner, detail::MinifyImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Minify<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::MinifyImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Minify1 = Minify<>;
}
