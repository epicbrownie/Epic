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
		struct BlendImpl;
	}

	template<class First, class Second>
	struct Blend;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class F, class S>
struct Epic::Math::XForm::detail::BlendImpl
{
	F BlendFirst;
	S BlendSecond;
	T Bias = T(0.5);

	constexpr T operator() (T t) const noexcept
	{
		const T w = T(Bias);
		const T f = BlendFirst(t);
		const T s = BlendSecond(t);

		return w * (s - f) + f;
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class F, class S>
struct Epic::Math::XForm::Blend
	: public detail::XFormImpl2<F, S, detail::BlendImpl> { };

template<template<class, class...> class F, class S, class... FArgs>
struct Epic::Math::XForm::Blend<F<FArgs...>, S>
	: public detail::XFormImpl2<F<FArgs...>, S, detail::BlendImpl> { };

template<class F, template<class, class...> class S, class... SArgs>
struct Epic::Math::XForm::Blend<F, S<SArgs...>>
	: public detail::XFormImpl2<F, S<SArgs...>, detail::BlendImpl>{};

template<
	template<class, class...> class F, 
	template<class, class...> class S,
	class... FArgs, class... SArgs>
struct Epic::Math::XForm::Blend<F<FArgs...>, S<SArgs...>>
	: public detail::XFormImpl2<F<FArgs...>, S<SArgs...>, detail::BlendImpl> { };
