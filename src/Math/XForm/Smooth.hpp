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
#include <Epic/Math/XForm/SmoothStart.hpp>
#include <Epic/Math/XForm/SmoothStop.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	template<size_t N, class Inner = Linear>
	struct Smooth;
}

//////////////////////////////////////////////////////////////////////////////
 
template<size_t N, class Inner>
struct Epic::Math::XForm::Smooth 
	: public Fade<SmoothStart<N, Inner>, SmoothStop<N, Inner>> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Smooth2 = Smooth<2>;
	using Smooth3 = Smooth<3>;
	using Smooth4 = Smooth<4>;
	using Smooth5 = Smooth<5>;
}
