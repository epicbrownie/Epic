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
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	static OnesMatrixTag OnesMatrix;
	static ZeroesMatrixTag ZeroesMatrix;
	static IdentityMatrixTag IdentityMatrix;

	static TranslationMatrixTag TranslationMatrix;
	static ScaleMatrixTag ScaleMatrix;
	static RotationMatrixTag RotationMatrix;
}
