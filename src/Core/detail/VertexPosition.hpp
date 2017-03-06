//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2017 Ronnie Brohn (EpicBrownie)      
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

#include <Epic/Core/VertexComponent.hpp>
#include <Epic/Math/Vector.hpp>

//////////////////////////////////////////////////////////////////////////////

MAKE_VERTEX_COMPONENT(CmpPos, Epic::Point4f, Pos, 4);
MAKE_VERTEX_COMPONENT(CmpPos2, Epic::Point4f, Pos2, 4);
MAKE_VERTEX_COMPONENT(CmpPos3, Epic::Point4f, Pos3, 4);
MAKE_VERTEX_COMPONENT(CmpPos4, Epic::Point4f, Pos4, 4);

namespace Epic::Component
{
	using CmpPos1 = CmpPos;
}
