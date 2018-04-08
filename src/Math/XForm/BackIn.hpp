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
		class BackInImpl;
	}

	template<size_t N, class Inner = Linear>
	struct BackIn;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
class Epic::Math::XForm::detail::BackInImpl
{
private:
	detail::BezierImpl<T, 3, detail::LinearImpl<T>> m_BezierFilter;

public:
	Inner BackInFilter;

public:
	BackInImpl()
	{
		m_BezierFilter.Controls[0] = -(T(0.5) * Power<N>(T(1.25)));
		m_BezierFilter.Controls[1] = T(1);
	}

public:
	inline T operator() (T t) const noexcept
	{
		const T tprime = BackInFilter(t);

		return m_BezierFilter(tprime);
	}
};

template<class T, size_t N>
class Epic::Math::XForm::detail::BackInImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
private:
	detail::BezierImpl<T, 3, detail::LinearImpl<T>> m_BezierFilter;

public:
	BackInImpl()
	{
		m_BezierFilter.Controls[0] = -(T(0.5) * Power<N>(T(1.25)));
		m_BezierFilter.Controls[1] = T(1);
	}

public:
	inline T operator() (T t) const noexcept
	{
		return m_BezierFilter(t);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::BackIn
	: public detail::XFormNImpl1<N, Inner, detail::BackInImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::BackIn<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::BackInImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using BackIn1 = BackIn<1>;
	using BackIn2 = BackIn<2>;
	using BackIn3 = BackIn<3>;
	using BackIn4 = BackIn<4>;
	using BackIn5 = BackIn<5>;
}
