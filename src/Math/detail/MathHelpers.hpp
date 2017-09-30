//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2016 Ronnie Brohn (EpicBrownie)      
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

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	struct MathOperationUnavailable { };
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	struct OnesTag { };
	struct ZeroesTag { };
	struct IdentityTag { };
	
	struct TranslationTag { };
	struct ScaleTag { };

	struct RotationTag { };
	struct XRotationTag { };
	struct YRotationTag { };
	struct ZRotationTag { };

	struct LookAtTag { };
	struct FrustumTag { };
	struct PerspectiveTag { };
	struct OrthoTag { };
	struct Ortho2DTag { };
	struct ShearTag { };
	struct PickingTag { };
	struct ShadowTag { };
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	static OnesTag			One;
	static ZeroesTag		Zero;
	static IdentityTag		Identity, Origin;

	static TranslationTag	Translation;
	static ScaleTag			Scale;

	static RotationTag		Rotation;
	static XRotationTag		XRotation;
	static YRotationTag		YRotation;
	static ZRotationTag		ZRotation;

	static LookAtTag		LookAt;
	static FrustumTag		Frustum;
	static PerspectiveTag	Perspective;
	static OrthoTag			Ortho;
	static Ortho2DTag		Ortho2D;
	static ShearTag			Shear;
	static PickingTag		Picking;
	static ShadowTag		Shadow;
}
