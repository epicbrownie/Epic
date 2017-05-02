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
	struct AttrColor1;
	struct AttrColor2;
	struct AttrColor3;
	struct AttrColor4;
}

//////////////////////////////////////////////////////////////////////////////

MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrColor1, Epic::Color4f, Color,  4, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrColor2, Epic::Color4f, Color2, 4, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrColor3, Epic::Color4f, Color3, 4, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrColor4, Epic::Color4f, Color4, 4, Epic::eComponentType::Float, false);

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Attribute
{
	using AttrColor = AttrColor1;
}
