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
#include <Epic/Math/XForm/SmoothStart.hpp>
#include <Epic/Math/XForm/SmoothStop.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, size_t N, class Inner>
		class SmoothImpl;
	}

	template<size_t N, class Inner = Linear>
	struct Smooth;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
class Epic::Math::XForm::detail::SmoothImpl
{
private:
	detail::SmoothStartImpl<T, N, detail::LinearImpl<T>> m_SmoothStart;
	detail::SmoothStopImpl<T, N, detail::LinearImpl<T>> m_SmoothStop;
	detail::FadeImpl<T, decltype(m_SmoothStart), decltype(m_SmoothStop)> m_Fade;

public:
	Inner SmoothFilter;

public:
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = SmoothFilter(t);

		return m_Fade(tprime);
	}
};

template<class T, size_t N>
class Epic::Math::XForm::detail::SmoothImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
private:
	detail::SmoothStartImpl<T, N, detail::LinearImpl<T>> m_SmoothStart;
	detail::SmoothStopImpl<T, N, detail::LinearImpl<T>> m_SmoothStop;
	detail::FadeImpl<T, decltype(m_SmoothStart), decltype(m_SmoothStop)> m_Fade;

public:
	constexpr T operator() (T t) const noexcept
	{
		return m_Fade(t);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::Smooth
	: public detail::XFormNImpl1<N, Inner, detail::SmoothImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Smooth<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::SmoothImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Smooth2 = Smooth<2>;
	using Smooth3 = Smooth<3>;
	using Smooth4 = Smooth<4>;
	using Smooth5 = Smooth<5>;
}
