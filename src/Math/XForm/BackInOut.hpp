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
#include <Epic/Math/XForm/Bezier.hpp>
#include <Epic/Math/XForm/Linear.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, size_t N, class Inner>
		class BackInOutImpl;
	}

	template<size_t N, class Inner = Linear>
	struct BackInOut;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
class Epic::Math::XForm::detail::BackInOutImpl
{
private:
	detail::BezierImpl<T, 4, detail::LinearImpl<T>> m_BezierFilter;

public:
	Inner BackInOutFilter;

public:
	BackInOutImpl()
	{
		const T bias = T(0.5) * Power<N>(T(1.25));

		m_BezierFilter.Controls[0] = -bias;
		m_BezierFilter.Controls[1] = T(0.5);
		m_BezierFilter.Controls[2] = T(1) + bias;
	}

public:
	inline T operator() (T t) const noexcept
	{
		const T tprime = BackInOutFilter(t);

		return m_BezierFilter(tprime);
	}
};

template<class T, size_t N>
class Epic::Math::XForm::detail::BackInOutImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
private:
	detail::BezierImpl<T, 4, detail::LinearImpl<T>> m_BezierFilter;

public:
	BackInOutImpl()
	{
		const T bias = T(0.5) * Power<N>(T(1.25));

		m_BezierFilter.Controls[0] = -bias;
		m_BezierFilter.Controls[1] = T(0.5);
		m_BezierFilter.Controls[2] = T(1) + bias;
	}

public:
	inline T operator() (T t) const noexcept
	{
		return m_BezierFilter(t);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::BackInOut
	: public detail::XFormNImpl1<N, Inner, detail::BackInOutImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::BackInOut<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::BackInOutImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using BackInOut1 = BackInOut<1>;
	using BackInOut2 = BackInOut<2>;
	using BackInOut3 = BackInOut<3>;
	using BackInOut4 = BackInOut<4>;
	using BackInOut5 = BackInOut<5>;
}
