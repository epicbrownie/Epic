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

MAKE_VERTEX_COMPONENT(CmpNormal, Epic::Normal3f, Normal, 3);
MAKE_VERTEX_COMPONENT(CmpTangent, Epic::Normal3f, Tangent, 3);
MAKE_VERTEX_COMPONENT(CmpBitangent, Epic::Normal3f, Bitangent, 3);
