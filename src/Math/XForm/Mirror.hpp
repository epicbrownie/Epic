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

#include <Epic/Math/XForm/MirrorBottom.hpp>
#include <Epic/Math/XForm/MirrorTop.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	template<class Inner>
	struct Mirror;
}

//////////////////////////////////////////////////////////////////////////////

template<class Inner>
struct Epic::Math::XForm::Mirror : public MirrorBottom<MirrorTop<Inner>> { };
