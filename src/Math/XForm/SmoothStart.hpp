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
		struct SmoothStartImpl;
	}

	template<size_t N, class Inner = Linear>
	struct SmoothStart;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
struct Epic::Math::XForm::detail::SmoothStartImpl
{
	Inner SmoothFilter;

	constexpr T operator() (T t) const noexcept
	{
		const T tprime = SmoothFilter(t);

		return Power<N>(tprime);
	}
};

template<class T, size_t N>
struct Epic::Math::XForm::detail::SmoothStartImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
	constexpr T operator() (T t) const noexcept
	{
		return Power<N>(t);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::SmoothStart
	: public detail::XFormNImpl1<N, Inner, detail::SmoothStartImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::SmoothStart<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::SmoothStartImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using SmoothStart2 = SmoothStart<2>;
	using SmoothStart3 = SmoothStart<3>;
	using SmoothStart4 = SmoothStart<4>;
	using SmoothStart5 = SmoothStart<5>;
}
