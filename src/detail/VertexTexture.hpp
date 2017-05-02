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
	struct Attr2DTex1; struct Attr2DTex2; struct Attr2DTex3; struct Attr2DTex4;
	struct Attr2DTex5; struct Attr2DTex6; struct Attr2DTex7; struct Attr2DTex8;
	struct Attr3DTex1; struct Attr3DTex2; struct Attr3DTex3; struct Attr3DTex4;
	struct Attr3DTex5; struct Attr3DTex6; struct Attr3DTex7; struct Attr3DTex8;
}

//////////////////////////////////////////////////////////////////////////////

MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr2DTex1, Epic::TexCoord2f, Texture,  2, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr2DTex2, Epic::TexCoord2f, Texture2, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr2DTex3, Epic::TexCoord2f, Texture3, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr2DTex4, Epic::TexCoord2f, Texture4, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr2DTex5, Epic::TexCoord2f, Texture5, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr2DTex6, Epic::TexCoord2f, Texture6, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr2DTex7, Epic::TexCoord2f, Texture7, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr2DTex8, Epic::TexCoord2f, Texture8, 2, Epic::eComponentType::Float, false);
					  
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr3DTex1, Epic::TexCoord3f, Texture,  3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr3DTex2, Epic::TexCoord3f, Texture2, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr3DTex3, Epic::TexCoord3f, Texture3, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr3DTex4, Epic::TexCoord3f, Texture4, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr3DTex5, Epic::TexCoord3f, Texture5, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr3DTex6, Epic::TexCoord3f, Texture6, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr3DTex7, Epic::TexCoord3f, Texture7, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_ATTRIBUTE(Epic::Attribute::Attr3DTex8, Epic::TexCoord3f, Texture8, 3, Epic::eComponentType::Float, false);

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Attribute
{
	using Attr2DTex = Attr2DTex1;
	using Attr3DTex = Attr3DTex1;
	
	using AttrTex = Attr2DTex;

	using AttrTex1 = Attr2DTex1;
	using AttrTex2 = Attr2DTex2;
	using AttrTex3 = Attr2DTex3;
	using AttrTex4 = Attr2DTex4;
	using AttrTex5 = Attr2DTex5;
	using AttrTex6 = Attr2DTex6;
	using AttrTex7 = Attr2DTex7;
	using AttrTex8 = Attr2DTex8;
}
