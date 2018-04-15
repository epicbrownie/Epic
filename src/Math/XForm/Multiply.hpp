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
		struct MultiplyImpl;
	}

	template<size_t N, class Inner = Linear>
	struct Multiply;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
struct Epic::Math::XForm::detail::MultiplyImpl
{
	Inner MultiplyInner;

	// NOTE: Equivalent to Scale<Inner>
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = MultiplyInner(t);

		return T(N) * tprime;
	}
};

template<class T, size_t N>
struct Epic::Math::XForm::detail::MultiplyImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
	// NOTE: Equivalent to Scale<Linear>
	constexpr T operator() (T t) const noexcept
	{
		return T(N) * t;
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::Multiply
	: public detail::XFormNImpl1<N, Inner, detail::MultiplyImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Multiply<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::MultiplyImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Multiply2 = Multiply<2>;
	using Multiply3 = Multiply<3>;
	using Multiply4 = Multiply<4>;
}
