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

#include <Epic/VertexComponent.hpp>
#include <Epic/Math/Vector.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Component
{
	struct CmpColor;
	struct CmpColor2;
	struct CmpColor3;
	struct CmpColor4;
}

//////////////////////////////////////////////////////////////////////////////

MAKE_VERTEX_COMPONENT(Epic::Component::CmpColor, Epic::Color4f, Color, 4, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::CmpColor2, Epic::Color4f, Color2, 4, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::CmpColor3, Epic::Color4f, Color3, 4, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::CmpColor4, Epic::Color4f, Color4, 4, Epic::eComponentType::Float, false);

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Component
{
	using CmpColor1 = CmpColor;
}
