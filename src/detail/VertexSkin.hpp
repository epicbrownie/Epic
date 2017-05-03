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
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Attribute
{
	struct AttrSkin1;
	struct AttrSkin2;
	struct AttrSkin3;
	struct AttrSkin4;

	struct AttrJoints1;
	struct AttrJoints2;
	struct AttrJoints3;
	struct AttrJoints4;
}

//////////////////////////////////////////////////////////////////////////////

MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrSkin1, Epic::Vector4f, Skin,  4, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrSkin2, Epic::Vector4f, Skin2, 4, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrSkin3, Epic::Vector4f, Skin3, 4, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrSkin4, Epic::Vector4f, Skin4, 4, Epic::eComponentType::Float, false);

MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrJoints1, Epic::Vector4<uint8_t>, Joints,  4, Epic::eComponentType::UByte, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrJoints2, Epic::Vector4<uint8_t>, Joints2, 4, Epic::eComponentType::UByte, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrJoints3, Epic::Vector4<uint8_t>, Joints3, 4, Epic::eComponentType::UByte, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::AttrJoints4, Epic::Vector4<uint8_t>, Joints4, 4, Epic::eComponentType::UByte, false);

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Attribute
{
	using AttrSkin = AttrSkin1;
	using AttrJoints = AttrJoints1;
}
