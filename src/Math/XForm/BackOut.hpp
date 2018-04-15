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
		class BackOutImpl;
	}

	template<size_t N, class Inner = Linear>
	struct BackOut;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
class Epic::Math::XForm::detail::BackOutImpl
{
private:
	detail::BezierImpl<T, 3, detail::LinearImpl<T>> m_BezierFilter;

public:
	Inner BackOutInner;

public:
	BackOutImpl()
	{
		m_BezierFilter.Controls[0] = T(0);
		m_BezierFilter.Controls[1] = T(1) + (T(0.5) * Power<N>(T(1.25)));
	}

public:
	inline T operator() (T t) const noexcept
	{
		const T tprime = BackOutInner(t);

		return m_BezierFilter(tprime);
	}
};

template<class T, size_t N>
class Epic::Math::XForm::detail::BackOutImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
private:
	detail::BezierImpl<T, 3, detail::LinearImpl<T>> m_BezierFilter;

public:
	BackOutImpl()
	{
		m_BezierFilter.Controls[0] = T(0);
		m_BezierFilter.Controls[1] = T(1) + (T(0.5) * Power<N>(T(1.25)));
	}

public:
	inline T operator() (T t) const noexcept
	{
		return m_BezierFilter(t);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::BackOut
	: public detail::XFormNImpl1<N, Inner, detail::BackOutImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::BackOut<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::BackOutImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using BackOut1 = BackOut<1>;
	using BackOut2 = BackOut<2>;
	using BackOut3 = BackOut<3>;
	using BackOut4 = BackOut<4>;
	using BackOut5 = BackOut<5>;
}
