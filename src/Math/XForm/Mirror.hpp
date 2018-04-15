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
#include <Epic/Math/XForm/MirrorTop.hpp>
#include <Epic/Math/XForm/MirrorBottom.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, class Inner>
		class MirrorImpl;
	}

	template<class Inner>
	struct Mirror;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, class Inner>
class Epic::Math::XForm::detail::MirrorImpl
{
private:
	detail::MirrorTopImpl<T, detail::LinearImpl<T>> m_MirrorTop;
	detail::MirrorBottomImpl<T, detail::LinearImpl<T>> m_MirrorBottom;

public:
	Inner MirrorFilter;

public:
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = MirrorFilter(t);

		return m_MirrorBottom(m_MirrorTop(tprime));
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Mirror
	: public detail::XFormImpl1<Inner, detail::MirrorImpl> { };

template<template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Mirror<Inner<InnerArgs...>>
	: public detail::XFormImpl1<Inner<InnerArgs...>, detail::MirrorImpl> { };
