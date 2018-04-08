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

#include <Epic/Math/XForm/Fade.hpp>
#include <Epic/Math/XForm/Linear.hpp>
#include <Epic/Math/XForm/SmoothStartSine.hpp>
#include <Epic/Math/XForm/SmoothStopSine.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	template<size_t N = 1, class Inner = Linear>
	struct SmoothSine;
}

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::SmoothSine
	: public Fade<SmoothStartSine<N, Inner>, SmoothStopSine<N, Inner>> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using SmoothSine1 = SmoothSine<1>;
	using SmoothSine2 = SmoothSine<2>;
	using SmoothSine3 = SmoothSine<3>;
}
