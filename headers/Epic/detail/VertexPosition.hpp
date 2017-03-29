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
	struct CmpPos;
	struct CmpPos2;
	struct CmpPos3;
	struct CmpPos4;
}

//////////////////////////////////////////////////////////////////////////////

MAKE_VERTEX_COMPONENT(Epic::Component::CmpPos, Epic::Point3f, Position, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::CmpPos2, Epic::Point3f, Position2, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::CmpPos3, Epic::Point3f, Position3, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::CmpPos4, Epic::Point3f, Position4, 3, Epic::eComponentType::Float, false);

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Component
{
	using CmpPos1 = CmpPos;
}
