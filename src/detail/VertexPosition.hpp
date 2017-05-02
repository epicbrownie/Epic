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
	struct AttrPos1;
	struct AttrPos2;
	struct AttrPos3;
	struct AttrPos4;
}

//////////////////////////////////////////////////////////////////////////////

MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrPos1, Epic::Point3f, Position,  3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrPos2, Epic::Point3f, Position2, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrPos3, Epic::Point3f, Position3, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrPos4, Epic::Point3f, Position4, 3, Epic::eComponentType::Float, false);

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Attribute
{
	using AttrPos = AttrPos1;
}
