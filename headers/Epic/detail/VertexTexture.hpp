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
	struct Cmp2DTex;  struct Cmp2DTex2; struct Cmp2DTex3; struct Cmp2DTex4;
	struct Cmp2DTex5; struct Cmp2DTex6;	struct Cmp2DTex7; struct Cmp2DTex8;
	struct Cmp3DTex;  struct Cmp3DTex2;	struct Cmp3DTex3; struct Cmp3DTex4;
	struct Cmp3DTex5; struct Cmp3DTex6;	struct Cmp3DTex7; struct Cmp3DTex8;
}

//////////////////////////////////////////////////////////////////////////////

MAKE_VERTEX_COMPONENT(Epic::Component::Cmp2DTex, Epic::TexCoord2f, Texture, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp2DTex2, Epic::TexCoord2f, Texture2, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp2DTex3, Epic::TexCoord2f, Texture3, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp2DTex4, Epic::TexCoord2f, Texture4, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp2DTex5, Epic::TexCoord2f, Texture5, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp2DTex6, Epic::TexCoord2f, Texture6, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp2DTex7, Epic::TexCoord2f, Texture7, 2, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp2DTex8, Epic::TexCoord2f, Texture8, 2, Epic::eComponentType::Float, false);
					  
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp3DTex, Epic::TexCoord3f, Texture, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp3DTex2, Epic::TexCoord3f, Texture2, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp3DTex3, Epic::TexCoord3f, Texture3, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp3DTex4, Epic::TexCoord3f, Texture4, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp3DTex5, Epic::TexCoord3f, Texture5, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp3DTex6, Epic::TexCoord3f, Texture6, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp3DTex7, Epic::TexCoord3f, Texture7, 3, Epic::eComponentType::Float, false);
MAKE_VERTEX_COMPONENT(Epic::Component::Cmp3DTex8, Epic::TexCoord3f, Texture8, 3, Epic::eComponentType::Float, false);

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Component
{
	using CmpTex = Cmp2DTex;

	using Cmp2DTex1 = Cmp2DTex;
	using Cmp3DTex1 = Cmp3DTex;

	using CmpTex1 = Cmp2DTex1;
	using CmpTex2 = Cmp2DTex2;
	using CmpTex3 = Cmp2DTex3;
	using CmpTex4 = Cmp2DTex4;
	using CmpTex5 = Cmp2DTex5;
	using CmpTex6 = Cmp2DTex6;
	using CmpTex7 = Cmp2DTex7;
	using CmpTex8 = Cmp2DTex8;
}
