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

#include <Epic/Singleton.hpp>
#include <Epic/detail/GLFWInclude.hpp>
#include <cassert>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	class GLFW;
}

//////////////////////////////////////////////////////////////////////////////

// GLFW
class Epic::detail::GLFW
{
public:
	using Type = Epic::detail::GLFW;

private:
	bool m_IsReady;

public:
	GLFW() noexcept
		: m_IsReady{ false }
	{ 
		glfwSetErrorCallback(&Type::OnError);
	}

	~GLFW() noexcept
	{
		if (m_IsReady)
			glfwTerminate();
	}

private:
	static void OnError(int errCode, const char* description) noexcept
	{
		std::cerr << "ERROR in GLFW"
				  << std::endl
				  << "\tError "
				  << errCode
				  << " - "
				  << description
				  << std::endl;
	}

public:
	bool Initialize() noexcept
	{
		m_IsReady = (glfwInit() == GLFW_TRUE);
		return m_IsReady;
	}
	
	inline bool IsReady() const noexcept 
	{ 
		return m_IsReady; 
	}
};
