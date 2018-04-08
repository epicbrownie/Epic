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
		struct MagnifyImpl;
	}

	template<class Inner = Linear>
	struct Magnify;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::MagnifyImpl
{
	Inner MagFilter;
	
	// NOTE: Equivalent to Modulate<Inverse, Inner>
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = MagFilter(t);

		return (T(1) - t) * tprime;
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Magnify
	: public detail::XFormImpl1<Inner, detail::MagnifyImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Magnify<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::MagnifyImpl> { };
