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
		struct CosineImpl;
	}

	template<class Inner = Angle<>>
	struct Cosine;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
struct Epic::Math::XForm::detail::CosineImpl
{
	Inner CosFilter;

	constexpr T operator() (T t) const noexcept
	{
		const T tprime = CosFilter(t);

		return std::cos(tprime);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Cosine
	: public detail::XFormImpl1<Inner, detail::CosineImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Cosine<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::CosineImpl> { };
