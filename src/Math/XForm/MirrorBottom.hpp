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
		struct MirrorBottomImpl;
	}

	template<class Inner>
	struct MirrorBottom;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::MirrorBottomImpl
{
	Inner MirrorFilter;

	constexpr T operator() (T t) const noexcept
	{
		const T tprime = MirrorFilter(t);
		
		if constexpr (std::is_arithmetic_v<T>)
			return abs(tprime);
		else
			return tprime < T(0) ? -tprime : tprime;
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::MirrorBottom
	: public detail::XFormImpl1<Inner, detail::MirrorBottomImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::MirrorBottom<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::MirrorBottomImpl> { };
