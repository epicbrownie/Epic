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
#include <Epic/Math/XForm/detail/BezierHelpers.hpp>
#include <Epic/Math/XForm/Linear.hpp>
#include <array>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, size_t N, class Inner>
		class BezierImpl;
	}

	template<size_t N, class Inner = Linear>
	struct Bezier;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
class Epic::Math::XForm::detail::BezierImpl
{
private:
	using Coefficients = BezierCoefficients<N>;

public:
	Inner BezierFilter;
	std::array<T, N - 1> Controls;

private:
	template<size_t I>
	inline T EvalTerms(T t, const std::array<T, N - 1>& Ts, const std::array<T, N - 1>& TIs) const noexcept
	{
		return (std::tuple_element_t<I + 1, Coefficients>::value 
			* Controls[I] 
			* Ts[I] 
			* TIs[N - I - 2]) + EvalTerms<I + 1>(t, Ts, TIs);
	}

	template<>
	inline T EvalTerms<N - 1>(T t, const std::array<T, N - 1>& Ts, const std::array<T, N - 1>&) const noexcept
	{
		return Ts[N - 2] * t;
	}

public:
	inline T operator() (T t) const noexcept
	{
		std::array<T, N - 1> Ts;
		std::array<T, N - 1> TIs;

		const T tprime = BezierFilter(t);
		const T ti = T(1) - tprime;

		Ts[0] = tprime;
		TIs[0] = ti;

		for (int i = 1; i < N - 1; ++i)
		{
			Ts[i] = Ts[i - 1] * tprime;
			TIs[i] = TIs[i - 1] * ti;
		}
		
		return EvalTerms<0>(tprime, Ts, TIs);
	}
};

template<class T, class Inner>
class Epic::Math::XForm::detail::BezierImpl<T, 1, Inner>
{
public:
	Inner BezierFilter;

public:
	inline T operator() (T t) const noexcept
	{
		return BezierFilter(t);
	}
};

template<class T, class Inner>
class Epic::Math::XForm::detail::BezierImpl<T, 0, Inner>
	: public Epic::Math::XForm::detail::BezierImpl<T, 1, Inner> { };

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::Bezier
	: public detail::XFormNImpl1<N, Inner, detail::BezierImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Bezier<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::BezierImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Bezier2 = Bezier<2>;
	using Bezier3 = Bezier<3>;
	using Bezier4 = Bezier<4>;
	using Bezier5 = Bezier<5>;
}