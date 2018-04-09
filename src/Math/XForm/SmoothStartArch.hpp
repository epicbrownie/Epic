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
	template<size_t N, class Inner = Linear>
	struct SmoothStartArch;
}

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::SmoothStartArch : public Arch<N, Minify<Inner>> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using SmoothStartArch2 = SmoothStartArch<2>;
	using SmoothStartArch3 = SmoothStartArch<3>;
}