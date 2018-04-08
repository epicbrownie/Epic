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
#include <Epic/Math/XForm/BackIn.hpp>
#include <Epic/Math/XForm/BackOut.hpp>
#include <Epic/Math/XForm/BackInOut.hpp>
#include <Epic/Math/XForm/Bell.hpp>
#include <Epic/Math/XForm/Bezier.hpp>
#include <Epic/Math/XForm/Blend.hpp>
#include <Epic/Math/XForm/Clamp.hpp>
#include <Epic/Math/XForm/Cosine.hpp>
#include <Epic/Math/XForm/Divide.hpp>
#include <Epic/Math/XForm/Fade.hpp>
#include <Epic/Math/XForm/Flip.hpp>
#include <Epic/Math/XForm/Hesitate.hpp>
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
