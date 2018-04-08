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

// Terminal
#include <Epic/Math/XForm/Constant.hpp>
#include <Epic/Math/XForm/Double.hpp>
#include <Epic/Math/XForm/Half.hpp>
#include <Epic/Math/XForm/Inverse.hpp>
#include <Epic/Math/XForm/Linear.hpp>

// Chainable
#include <Epic/Math/XForm/Angle.hpp>
#include <Epic/Math/XForm/Arch.hpp>
#include <Epic/Math/XForm/Bell.hpp>
#include <Epic/Math/XForm/Bezier.hpp>
#include <Epic/Math/XForm/Blend.hpp>
#include <Epic/Math/XForm/Clamp.hpp>
#include <Epic/Math/XForm/Cosine.hpp>
#include <Epic/Math/XForm/Divide.hpp>
#include <Epic/Math/XForm/Fade.hpp>
#include <Epic/Math/XForm/Flip.hpp>
#include <Epic/Math/XForm/Magnify.hpp>
#include <Epic/Math/XForm/Map.hpp>
#include <Epic/Math/XForm/Minify.hpp>
#include <Epic/Math/XForm/MirrorBottom.hpp>
#include <Epic/Math/XForm/MirrorTop.hpp>
#include <Epic/Math/XForm/Multiply.hpp>
#include <Epic/Math/XForm/Modulate.hpp>
#include <Epic/Math/XForm/Scale.hpp>
#include <Epic/Math/XForm/Sine.hpp>
#include <Epic/Math/XForm/SmoothStart.hpp>
#include <Epic/Math/XForm/SmoothStop.hpp>
#include <Epic/Math/XForm/SmoothStartSine.hpp>
#include <Epic/Math/XForm/SmoothStopSine.hpp>

// Composite
#include <Epic/Math/XForm/Mirror.hpp>
#include <Epic/Math/XForm/Smooth.hpp>
#include <Epic/Math/XForm/SmoothSine.hpp>
#include <Epic/Math/XForm/SmoothStartArch.hpp>
#include <Epic/Math/XForm/SmoothStopArch.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	template<class Desc, class T = float>
	using XForm = typename detail::ImplOf<Desc, T>::Type;
}

//		inline float BackIn(float t) const noexcept
//		{
//			const float s = 1.70158f;
//			return t * t * ((s + 1.0f) * t - s);
//		}
//
//		inline float BackOut(float t) const noexcept
//		{
//			const float s = 1.70158f;
//			const float x = t - 1.0f;
//			return x * x * ((s + 1.0f) * x + s) + 1.0f;
//		}
//
//		inline float BackInOut(float t) const noexcept
//		{
//			return (t > 0.5f) ?
//				0.5f + BackOut(t * 2.0f - 1.0f) * 0.5f :
//				BackIn(t * 2.0f) * 0.5f;
//		}
//
//		inline float BounceOut(float t) const noexcept
//		{
//			const float s = 7.5625f;
//			const float t1 = 1.0f / 2.75f;
//			const float t2 = 1.5f / 2.75f;
//			const float t3 = 2.0f / 2.75f;
//			const float m1 = 2.25f / 2.75f;
//			const float m2 = 2.5f / 2.75f;
//			const float m3 = 2.625f / 2.75f;
//			const float b1 = 0.75f;
//			const float b2 = 0.9375f;
//			const float b3 = 0.984375f;
//
//			return (t < t1) ? s * t * t :
//				(t < t2) ? s * (t - m1) * (t - m1) + b1 :
//				(t < t3) ? s * (t - m2) * (t - m2) + b2 :
//				s * (t - m3) * (t - m3) + b3;
//		}
//	}
//}