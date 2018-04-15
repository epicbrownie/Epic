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
#include <Epic/Math/Algorithm.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, size_t N, class Inner>
		struct SmoothStopSineImpl;
	}

	template<size_t N = 1, class Inner = Linear>
	struct SmoothStopSine;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
struct Epic::Math::XForm::detail::SmoothStopSineImpl
{
	Inner SmoothInner;

	// NOTE: Equivalent to Sine<Angle<Divide<2, SmoothStop<N, Inner>>>>
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = SmoothInner(t);

		return std::sin(HalfPi<T> * (T(1) - Power<N>(T(1) - tprime)));
	}
};

template<class T, size_t N>
struct Epic::Math::XForm::detail::SmoothStopSineImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
	// NOTE: Equivalent to Sine<Angle<Divide<2, SmoothStop<N, Linear>>>>
	constexpr T operator() (T t) const noexcept
	{
		return std::sin(HalfPi<T> * (T(1) - Power<N>(T(1) - t)));
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::SmoothStopSine
	: public detail::XFormNImpl1<N, Inner, detail::SmoothStopSineImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::SmoothStopSine<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::SmoothStopSineImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using SmoothStopSine1 = SmoothStopSine<1>;
	using SmoothStopSine2 = SmoothStopSine<2>;
	using SmoothStopSine3 = SmoothStopSine<3>;
}
