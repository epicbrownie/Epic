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
#include <cmath>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, class Inner>
		struct MirrorTopImpl;
	}

	template<class Inner>
	struct MirrorTop;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::MirrorTopImpl
{
	Inner MirrorInner;

	constexpr T operator() (T t) const noexcept
	{
		const T tprime = T(1) - MirrorInner(t);

		if constexpr (std::is_arithmetic_v<T>)
			return T(1) - abs(tprime);
		else
			return T(1) - (tprime < T(0) ? -tprime : tprime);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::MirrorTop
	: public detail::XFormImpl1<Inner, detail::MirrorTopImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::MirrorTop<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::MirrorTopImpl> { };
