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

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, class F, class S>
		struct FadeImpl;
	}

	template<class First, class Second>
	struct Fade;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class F, class S>
struct Epic::Math::XForm::detail::FadeImpl
{
	F FadeFirst;
	S FadeSecond;

	constexpr T operator() (T t) const noexcept
	{
		const T f = FadeFirst(t);
		const T s = FadeSecond(t);

		return t * (s - f) + f;
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class F, class S>
struct Epic::Math::XForm::Fade
	: public detail::XFormImpl2<F, S, detail::FadeImpl> { };

template<template<class...> class F, class S, class... FArgs>
struct Epic::Math::XForm::Fade<F<FArgs...>, S>
	: public detail::XFormImpl2<F<FArgs...>, S, detail::FadeImpl> { };

template<class F, template<class...> class S, class... SArgs>
struct Epic::Math::XForm::Fade<F, S<SArgs...>>
	: public detail::XFormImpl2<F, S<SArgs...>, detail::FadeImpl> { };

template<
	template<class...> class F, template<class...> class S,
	class... FArgs, class... SArgs>
struct Epic::Math::XForm::Fade<F<FArgs...>, S<SArgs...>>
	: public detail::XFormImpl2<F<FArgs...>, S<SArgs...>, detail::FadeImpl> { };
