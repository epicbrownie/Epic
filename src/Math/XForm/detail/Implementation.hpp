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

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm::detail
{
	template<template<class> class Impl, class T>
	struct ImplOf2;

	template<class Desc, class T>
	struct ImplOf;

	template<template<class> class Impl>
	struct XFormImpl0;

	template<class Inner1, template<class, class> class Impl>
	struct XFormImpl1;

	template<class Inner1, class Inner2, template<class, class, class> class Impl>
	struct XFormImpl2;

	template<size_t N, class Inner1, template<class, size_t, class> class Impl>
	struct XFormNImpl1;
}

//////////////////////////////////////////////////////////////////////////////

template<template<class> class Impl, class T>
struct Epic::Math::XForm::detail::ImplOf2
{
	using Type = Impl<T>;
};

template<class Desc, class T>
struct Epic::Math::XForm::detail::ImplOf
{
	using Type = typename ImplOf2<Desc::Impl, T>::Type;
};

//////////////////////////////////////////////////////////////////////////////

template<template<class> class ImplType>
struct Epic::Math::XForm::detail::XFormImpl0
{
	template<class T>
	using Impl = ImplType<T>;
};

template<class Inner, template<class, class> class ImplType>
struct Epic::Math::XForm::detail::XFormImpl1
{
	template<class T>
	using InnerImpl = typename ImplOf<Inner, T>::Type;

	template<class T>
	using Impl = ImplType<T, InnerImpl<T>>;
};

template<class Inner1, class Inner2, template<class, class, class> class ImplType>
struct Epic::Math::XForm::detail::XFormImpl2
{
	template<class T>
	using Inner1Impl = typename ImplOf<Inner1, T>::Type;

	template<class T>
	using Inner2Impl = typename ImplOf<Inner2, T>::Type;

	template<class T>
	using Impl = ImplType<T, Inner1Impl<T>, Inner2Impl<T>>;
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner, template<class, size_t, class> class ImplType>
struct Epic::Math::XForm::detail::XFormNImpl1
{
	template<class T>
	using InnerImpl = typename ImplOf<Inner, T>::Type;

	template<class T>
	using Impl = ImplType<T, N, InnerImpl<T>>;
};
