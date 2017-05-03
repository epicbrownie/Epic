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

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////

#define FMODCHECK(_result) Epic::detail::CheckFMODResult(_result, __FILE__, __LINE__)

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		namespace
		{
			inline bool CheckFMODResult(FMOD_RESULT errCode, const char* file, int line) noexcept
			{
				if (errCode != FMOD_OK)
				{
					std::cerr << "ERROR in \""
							  << file
							  << "\" on line "
							  << line
							  << std::endl
							  << "\tError "
							  << errCode
							  << " - "
							  << FMOD_ErrorString(errCode)
							  << std::endl;

					return false;
				}

				return true;
			}
		}
	}
}

