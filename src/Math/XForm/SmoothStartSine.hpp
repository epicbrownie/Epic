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
		struct SmoothStartSineImpl;
	}

	template<size_t N = 1, class Inner = Linear>
	struct SmoothStartSine;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
struct Epic::Math::XForm::detail::SmoothStartSineImpl
{
	Inner SmoothInner;

	// NOTE: Equivalent to Flip<Cosine<Angle<Divide<2, SmoothStart<N, Inner>>>>>
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = SmoothInner(t);
		
		return T(1) - std::cos(HalfPi<T> * Power<N>(tprime));
	}
};

template<class T, size_t N>
struct Epic::Math::XForm::detail::SmoothStartSineImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
	// NOTE: Equivalent to Flip<Cosine<Angle<Divide<2, SmoothStart<N, Linear>>>>>
	constexpr T operator() (T t) const noexcept
	{
		return T(1) - std::cos(HalfPi<T> * Power<N>(t));
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::SmoothStartSine
	: public detail::XFormNImpl1<N, Inner, detail::SmoothStartSineImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::SmoothStartSine<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::SmoothStartSineImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using SmoothStartSine1 = SmoothStartSine<1>;
	using SmoothStartSine2 = SmoothStartSine<2>;
	using SmoothStartSine3 = SmoothStartSine<3>;
}
