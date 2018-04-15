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

#include <Epic/Math/XForm/Fade.hpp>
#include <Epic/Math/XForm/Linear.hpp>
#include <Epic/Math/XForm/SmoothStartSine.hpp>
#include <Epic/Math/XForm/SmoothStopSine.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, size_t N, class Inner>
		class SmoothSineImpl;
	}

	template<size_t N, class Inner = Linear>
	struct SmoothSine;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
class Epic::Math::XForm::detail::SmoothSineImpl
{
private:
	detail::SmoothStartSineImpl<T, N, detail::LinearImpl<T>> m_SmoothStartSine;
	detail::SmoothStopSineImpl<T, N, detail::LinearImpl<T>> m_SmoothStopSine;
	detail::FadeImpl<T, decltype(m_SmoothStartSine), decltype(m_SmoothStopSine)> m_Fade;

public:
	Inner SmoothInner;

public:
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = SmoothInner(t);

		return m_Fade(tprime);
	}
};

template<class T, size_t N>
class Epic::Math::XForm::detail::SmoothSineImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
private:
	detail::SmoothStartSineImpl<T, N, detail::LinearImpl<T>> m_SmoothStartSine;
	detail::SmoothStopSineImpl<T, N, detail::LinearImpl<T>> m_SmoothStopSine;
	detail::FadeImpl<T, decltype(m_SmoothStartSine), decltype(m_SmoothStopSine)> m_Fade;

public:
	constexpr T operator() (T t) const noexcept
	{
		return m_Fade(t);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::SmoothSine
	: public detail::XFormNImpl1<N, Inner, detail::SmoothSineImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::SmoothSine<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::SmoothSineImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using SmoothSine2 = SmoothSine<2>;
	using SmoothSine3 = SmoothSine<3>;
	using SmoothSine4 = SmoothSine<4>;
	using SmoothSine5 = SmoothSine<5>;
}
