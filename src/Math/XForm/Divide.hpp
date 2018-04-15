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

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, size_t N, class Inner>
		struct DivideImpl;
	}

	template<size_t N, class Inner = Linear>
	struct Divide;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
struct Epic::Math::XForm::detail::DivideImpl
{
	Inner DivideInner;

	// NOTE: Equivalent to Scale<Inner>
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = DivideInner(t);

		return tprime / T(N);
	}
};

template<class T, size_t N>
struct Epic::Math::XForm::detail::DivideImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
	// NOTE: Equivalent to Scale<Linear>
	constexpr T operator() (T t) const noexcept
	{
		return t / T(N);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::Divide
	: public detail::XFormNImpl1<N, Inner, detail::DivideImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Divide<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::DivideImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Divide2 = Divide<2>;
	using Divide3 = Divide<3>;
	using Divide4 = Divide<4>;
}
