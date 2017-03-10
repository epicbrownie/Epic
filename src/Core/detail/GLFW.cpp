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

#include <Epic/Core/detail/GLFW.hpp>

//////////////////////////////////////////////////////////////////////////////

Epic::detail::GLFWFacade::GLFWFacade() noexcept
	: m_IsReady{ false }
{
}

Epic::detail::GLFWFacade::~GLFWFacade() noexcept
{
	if (m_IsReady)
		glfwTerminate();
}

bool Epic::detail::GLFWFacade::Initialize() noexcept
{
	m_IsReady = (glfwInit() == GLFW_TRUE);
	return m_IsReady;
}