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
	struct CmpNormal;
	struct CmpTangent;
	struct CmpBitangent;
}

//////////////////////////////////////////////////////////////////////////////

MAKE_VERTEX_COMPONENT(Epic::Component::CmpNormal, Epic::Normal3f, Normal, 3);
MAKE_VERTEX_COMPONENT(Epic::Component::CmpTangent, Epic::Normal3f, Tangent, 3);
MAKE_VERTEX_COMPONENT(Epic::Component::CmpBitangent, Epic::Normal3f, Bitangent, 3);
