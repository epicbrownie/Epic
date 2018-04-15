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

#include <Epic/Math/XForm/Arch.hpp>
#include <Epic/Math/XForm/Minify.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, size_t N, class Inner>
		class SmoothStartArchImpl;
	}

	template<size_t N, class Inner = Linear>
	struct SmoothStartArch;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
class Epic::Math::XForm::detail::SmoothStartArchImpl
{
private:
	detail::ArchImpl<T, N, detail::LinearImpl<T>> m_Arch;
	detail::MinifyImpl<T, detail::LinearImpl<T>> m_Minify;
	
public:
	Inner SmoothArchInner;

public:
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = SmoothArchInner(t);

		return m_Arch(m_Minify(tprime));
	}
};

template<class T, size_t N>
class Epic::Math::XForm::detail::SmoothStartArchImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
private:
	detail::ArchImpl<T, N, detail::LinearImpl<T>> m_Arch;
	detail::MinifyImpl<T, detail::LinearImpl<T>> m_Minify;

public:
	constexpr T operator() (T t) const noexcept
	{
		return m_Arch(m_Minify(t));
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::SmoothStartArch
	: public detail::XFormNImpl1<N, Inner, detail::SmoothStartArchImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::SmoothStartArch<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::SmoothStartArchImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using SmoothStartArch2 = SmoothStartArch<2>;
	using SmoothStartArch3 = SmoothStartArch<3>;
}