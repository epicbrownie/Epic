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

#include <Epic/Core/detail/GLFWInclude.h>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	class GLFWFacade;
}

//////////////////////////////////////////////////////////////////////////////

// GLFWFacade
class Epic::detail::GLFWFacade
{
private:
	bool m_IsReady;

public:
	GLFWFacade() noexcept;
	~GLFWFacade() noexcept;

public:
	bool Initialize() noexcept;
	
	inline bool IsReady() const noexcept 
	{ 
		return m_IsReady; 
	}
};
