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
		struct ModulateImpl;
	}

	template<class First, class Second>
	struct Modulate;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class F, class S>
struct Epic::Math::XForm::detail::ModulateImpl
{
	F ModFirst;
	S ModSecond;

	constexpr T operator() (T t) const noexcept
	{
		const T f = ModFirst(t);
		const T s = ModSecond(t);

		return f * s;
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class F, class S>
struct Epic::Math::XForm::Modulate
	: public detail::XFormImpl2<F, S, detail::ModulateImpl> { };

template<template<class...> class F, class S, class... FArgs>
struct Epic::Math::XForm::Modulate<F<FArgs...>, S>
	: public detail::XFormImpl2<F<FArgs...>, S, detail::ModulateImpl> { };

template<class F, template<class...> class S, class... SArgs>
struct Epic::Math::XForm::Modulate<F, S<SArgs...>>
	: public detail::XFormImpl2<F, S<SArgs...>, detail::ModulateImpl> { };

template<
	template<class...> class F, template<class...> class S,
	class... FArgs, class... SArgs>
struct Epic::Math::XForm::Modulate<F<FArgs...>, S<SArgs...>>
	: public detail::XFormImpl2<F<FArgs...>, S<SArgs...>, detail::ModulateImpl> { };
