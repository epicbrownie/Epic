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
		template<class T, class CustomType, class Inner>
		struct CustomImpl;
	}

	template<template<class> class CustomType, class Inner = Linear>
	struct Custom;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class CustomType, class Inner>
struct Epic::Math::XForm::detail::CustomImpl
{
	Inner InnerFilter;
	CustomType CustomFilter;

	inline T operator() (T t) const noexcept
	{
		const T tprime = InnerFilter(t);

		return CustomFilter(tprime);
	}
};

template<class T, class CustomType>
struct Epic::Math::XForm::detail::CustomImpl<T, CustomType, Epic::Math::XForm::detail::LinearImpl<T>>
{
	CustomType CustomFilter;

	inline T operator() (T t) const noexcept
	{
		return CustomFilter(t);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<template<class> class CustomType, class Inner>
struct Epic::Math::XForm::Custom
{
	template<class T>
	using InnerImpl = typename detail::ImplOf<Inner, T>::Type;

	template<class T>
	using Impl = detail::CustomImpl<T, CustomType<T>, InnerImpl<T>>;
};

template<template<class> class CustomType, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Custom<CustomType, Inner<InnerArgs...>>
{
	template<class T>
	using InnerImpl = typename detail::ImplOf<Inner<InnerArgs...>, T>::Type;

	template<class T>
	using Impl = detail::CustomImpl<T, CustomType<T>, InnerImpl<T>>;
};
