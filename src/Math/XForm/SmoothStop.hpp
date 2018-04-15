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
		struct SmoothStopImpl;
	}

	template<size_t N, class Inner = Linear>
	struct SmoothStop;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
struct Epic::Math::XForm::detail::SmoothStopImpl
{
	Inner SmoothInner;

	// NOTE: Equivalent to Flip<SmoothStart<Flip<Inner>>>
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = SmoothInner(t);
		
		return T(1) - Power<N>(T(1) - tprime);
	}
};

template<class T, size_t N>
struct Epic::Math::XForm::detail::SmoothStopImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
	// NOTE: Equivalent to Flip<SmoothStart<Inverse>>
	constexpr T operator() (T t) const noexcept
	{
		return T(1) - Power<N>((T)1 - t);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::SmoothStop
	: public detail::XFormNImpl1<N, Inner, detail::SmoothStopImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::SmoothStop<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner, detail::SmoothStopImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using SmoothStop2 = SmoothStop<2>;
	using SmoothStop3 = SmoothStop<3>;
	using SmoothStop4 = SmoothStop<4>;
	using SmoothStop5 = SmoothStop<5>;
}
