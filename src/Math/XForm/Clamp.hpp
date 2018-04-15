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
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, class Inner>
		struct ClampImpl;
	}

	template<class Inner>
	struct Clamp;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::ClampImpl
{
	Inner ClampInner;
	T Min = T(0);
	T Max = T(1);
	
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = ClampInner(t);

		return std::min(Max, std::max(Min, tprime));
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Clamp
	: public detail::XFormImpl1<Inner, detail::ClampImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Clamp<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::ClampImpl> { };
