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

#include <Epic/Math/Vector.hpp>
#include <Epic/Math/SVector.hpp>
#include <Epic/Math/Matrix.hpp>
#include <Epic/Math/Algorithm.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	// GLSL Vector Aliases

	using vec2 = Vector<float, 2>;
	using vec3 = Vector<float, 3>;
	using vec4 = Vector<float, 4>;

	using dvec2 = Vector<double, 2>;
	using dvec3 = Vector<double, 3>;
	using dvec4 = Vector<double, 4>;

	using ivec2 = Vector<std::int32_t, 2>;
	using ivec3 = Vector<std::int32_t, 3>;
	using ivec4 = Vector<std::int32_t, 4>;

	using uvec2 = Vector<std::uint32_t, 2>;
	using uvec3 = Vector<std::uint32_t, 3>;
	using uvec4 = Vector<std::uint32_t, 4>;

	using bvec2 = Vector<bool, 2>;
	using bvec3 = Vector<bool, 3>;
	using bvec4 = Vector<bool, 4>;

	
	// GLSL Matrix Aliases

	using mat2 = Matrix<float, 2>;
	using mat3 = Matrix<float, 3>;
	using mat4 = Matrix<float, 4>;
				  
	using dmat2 = Matrix<double, 2>;
	using dmat3 = Matrix<double, 3>;
	using dmat4 = Matrix<double, 4>;


	// Algorithm Imports

	using std::min;
	using std::max;
	using std::clamp;
}
