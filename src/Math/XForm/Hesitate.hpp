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
		class HesitateImpl;
	}

	template<size_t N, class Inner = Linear>
	struct Hesitate;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
class Epic::Math::XForm::detail::HesitateImpl
{
private:
	detail::BezierImpl<T, N + 2, detail::LinearImpl<T>> m_BezierFilter;

public:
	Inner HesitateInner;

public:
	HesitateImpl()
	{
		for (int i = 0; i < N + 1; ++i)
			m_BezierFilter.Controls[i] = T(0.5);
	}

public:
	inline T operator() (T t) const noexcept
	{
		const T tprime = HesitateInner(t);

		return m_BezierFilter(tprime);
	}
};

template<class T, size_t N>
class Epic::Math::XForm::detail::HesitateImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
private:
	detail::BezierImpl<T, N + 2, detail::LinearImpl<T>> m_BezierFilter;

public:
	HesitateImpl()
	{
		for (int i = 0; i < N + 1; ++i)
			m_BezierFilter.Controls[i] = T(0.5);
	}

public:
	inline T operator() (T t) const noexcept
	{
		return m_BezierFilter(t);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::Hesitate
	: public detail::XFormNImpl1<N, Inner, detail::HesitateImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Hesitate<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::HesitateImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Hesitate1 = Hesitate<1>;
	using Hesitate2 = Hesitate<2>;
	using Hesitate3 = Hesitate<3>;
	using Hesitate4 = Hesitate<4>;
	using Hesitate5 = Hesitate<5>;
}
