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

MAKE_VERTEX_COMPONENT(CmpColor, Epic::Color4f, Color, 4);
MAKE_VERTEX_COMPONENT(CmpColor2, Epic::Color4f, Color2, 4);
MAKE_VERTEX_COMPONENT(CmpColor3, Epic::Color4f, Color3, 4);
MAKE_VERTEX_COMPONENT(CmpColor4, Epic::Color4f, Color4, 4);

namespace Epic::Component
{
	using CmpColor1 = CmpColor;
}
