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
#include <Epic/Math/XForm/Filter.hpp>
#include <Epic/Math/XForm/XForms.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	template<class Descriptor, class T = float>
	using XForm = typename detail::ImplOf<Descriptor, T>::Type;

	template<class Descriptor, class T = float>
	class XFormFilter;
}

//////////////////////////////////////////////////////////////////////////////

template<class Descriptor, class T>
class Epic::Math::XForm::XFormFilter :
	public IFilter<T>,
	public XForm<Descriptor, T>
{
private:
	using XFormBase = XForm<Descriptor, T>;

public:
	inline T Apply(T t) const noexcept override
	{
		return XFormBase::operator() (t);
	}
};

//////////////////////////////////////////////////////////////////////////////

// Dynamic Aliases
namespace Epic::Math::XForm
{
	template<template<class> class CustomType>
	using DynamicCustom = Custom<CustomType, Dynamic>;

	template<template<class> class CustomType, class T = float>
	using DynamicCustomFilter = XFormFilter<Custom<CustomType, Dynamic>, T>;

	using DynamicBlend = Blend<Dynamic, Dynamic>;
	template<class T = float> using DynamicBlendFilter = XFormFilter<Blend<Dynamic, Dynamic>, T>;

	using DynamicClamp = Clamp<Dynamic>;
	template<class T = float> using DynamicClampFilter = XFormFilter<Clamp<Dynamic>, T>;

	using DynamicFade = Fade<Dynamic, Dynamic>;
	template<class T = float> using DynamicFadeFilter = XFormFilter<Fade<Dynamic, Dynamic>, T>;

	using DynamicFlip = Flip<Dynamic>;
	template<class T = float> using DynamicFlipFilter = XFormFilter<Flip<Dynamic>, T>;

	using DynamicMagnify = Magnify<Dynamic>;
	template<class T = float> using DynamicMagnifyFilter = XFormFilter<Magnify<Dynamic>, T>;

	using DynamicMinify = Minify<Dynamic>;
	template<class T = float> using DynamicMinifyFilter = XFormFilter<Minify<Dynamic>, T>;

	using DynamicMap = Map<Dynamic>;
	template<class T = float> using DynamicMapFilter = XFormFilter<Map<Dynamic>, T>;

	using DynamicMirror = Mirror<Dynamic>;
	template<class T = float> using DynamicMirrorFilter = XFormFilter<Mirror<Dynamic>, T>;

	using DynamicModulate = Modulate<Dynamic, Dynamic>;
	template<class T = float> using DynamicModulateFilter = XFormFilter<Modulate<Dynamic, Dynamic>, T>;

	using DynamicScale = Scale<Dynamic>;
	template<class T = float> using DynamicScaleFilter = XFormFilter<Scale<Dynamic>, T>;

	template<size_t N> using DynamicBezier = Bezier<N, Dynamic>;
	template<size_t N, class T = float> using DynamicBezierFilter = XFormFilter<Bezier<N, Dynamic>, T>;

	template<size_t N> using DynamicSmooth = Smooth<N, Dynamic>;
	template<size_t N, class T = float> using DynamicSmoothFilter = XFormFilter<Bezier<N, Dynamic>, T>;

	template<size_t N> using DynamicSmoothStart = SmoothStart<N, Dynamic>;
	template<size_t N, class T = float> using DynamicSmoothStartFilter = XFormFilter<SmoothStart<N, Dynamic>, T>;

	template<size_t N> using DynamicSmoothStop = SmoothStop<N, Dynamic>;
	template<size_t N, class T = float> using DynamicSmoothStopFilter = XFormFilter<SmoothStop<N, Dynamic>, T>;
}
