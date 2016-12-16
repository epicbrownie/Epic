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

namespace Epic
{
	struct OnesMatrixTag { };
	struct ZeroesMatrixTag { };
	struct IdentityMatrixTag { };
	
	struct TranslationMatrixTag { };
	struct ScaleMatrixTag { };

	struct RotationMatrixTag { };
	struct XRotationMatrixTag { };
	struct YRotationMatrixTag { };
	struct ZRotationMatrixTag { };

	struct LookAtMatrixTag { };
	struct FrustumMatrixTag { };
	struct PerspectiveMatrixTag { };
	struct OrthoMatrixTag { };
	struct Ortho2DMatrixTag { };
	struct ShearMatrixTag { };
	struct PickingMatrixTag { };
	struct ShadowMatrixTag { };
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	static OnesMatrixTag OneMatrix;
	static ZeroesMatrixTag ZeroMatrix;
	static IdentityMatrixTag IdentityMatrix;

	static TranslationMatrixTag TranslationMatrix;
	static ScaleMatrixTag ScaleMatrix;

	static RotationMatrixTag RotationMatrix;
	static XRotationMatrixTag XRotationMatrix;
	static YRotationMatrixTag YRotationMatrix;
	static ZRotationMatrixTag ZRotationMatrix;

	static LookAtMatrixTag LookAtMatrix;
	static FrustumMatrixTag FrustumMatrix;
	static PerspectiveMatrixTag PerspectiveMatrix;
	static OrthoMatrixTag OrthoMatrix;
	static Ortho2DMatrixTag Ortho2DMatrix;
	static ShearMatrixTag ShearMatrix;
	static PickingMatrixTag PickingMatrix;
	static ShadowMatrixTag ShadowMatrix;
}
