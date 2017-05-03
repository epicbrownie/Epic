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

#define GLFW_DLL
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////

#define GLEWCHECK(_result) Epic::detail::CheckGLEWResult(_result, __FILE__, __LINE__)

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		namespace
		{
			inline bool CheckGLEWResult(GLenum errCode, const char* file, int line) noexcept
			{
				if (errCode != GLEW_OK)
				{
					std::cerr << "ERROR in \""
						<< file
						<< "\" on line "
						<< line
						<< std::endl
						<< "\tError "
						<< errCode
						<< " - "
						<< glewGetErrorString(errCode)
						<< std::endl;

					return false;
				}

				return true;
			}
		}
	}
}
