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

#include <Epic/VertexAttribute.hpp>
#include <Epic/Math/Vector.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Attribute
{
	struct AttrNormal;
	struct AttrTangent;
	struct AttrBitangent;
}

//////////////////////////////////////////////////////////////////////////////

MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrNormal,    Epic::Normal3f, Normal,    3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrTangent,   Epic::Normal3f, Tangent,   3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrBitangent, Epic::Normal3f, Bitangent, 3, Epic::eComponentType::Float, false);
