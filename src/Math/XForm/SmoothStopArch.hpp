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
#include <Epic/Math/XForm/Flip.hpp>
#include <Epic/Math/XForm/Linear.hpp>
#include <Epic/Math/XForm/Magnify.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	template<size_t N, class Inner = Linear>
	struct SmoothStopArch;
}

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::SmoothStopArch : public Arch<N, Magnify<Flip<Inner>>> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using SmoothStopArch2 = SmoothStopArch<2>;
	using SmoothStopArch3 = SmoothStopArch<3>;
}