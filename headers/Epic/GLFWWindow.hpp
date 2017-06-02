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

#include <Epic/detail/GLFW.hpp>
#include <Epic/Event.hpp>
#include <Epic/GLFWContextTypes.hpp>
#include <Epic/Window.hpp>
#include <Epic/Singleton.hpp>
#include <cassert>
#include <cstdint>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class GLFWWindow;
}

//////////////////////////////////////////////////////////////////////////////

// GLFWWindow
class Epic::GLFWWindow : public Epic::Window
{
	using Type = Epic::GLFWWindow;
	using Base = Epic::Window;

private:
	GLFWContextSettings m_ContextSettings;
	GLFWwindow* m_pWindow;

public:
	GLFWWindow(WindowSettings& wndSettings, GLFWContextSettings& contextSettings) noexcept 
		: Window(wndSettings), m_ContextSettings{ contextSettings }, m_pWindow{ nullptr }
	{ }
	
	~GLFWWindow() noexcept 
	{ 
		Destroy(); 
	}

private:
	GLFWWindow(const Type&) = delete;
	Type& operator = (const Type&) = delete;

private:
	#pragma region GLFW Callbacks
	
	static void _CloseCallback(GLFWwindow* pGLFWWnd)
	{
		auto pWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(pGLFWWnd));
		pWindow->Close();
	}

	static void _FramebufferSizeCallback(GLFWwindow* pGLFWWnd, int width, int height)
	{
		auto pWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(pGLFWWnd));
		pWindow->m_Settings.ClientSize.Width = static_cast<WindowSize::ValueType>(width);
		pWindow->m_Settings.ClientSize.Height = static_cast<WindowSize::ValueType>(height);
		pWindow->OnWindowSizeChanged();
	}

	static void _PositionCallback(GLFWwindow* pGLFWWnd, int x, int y)
	{
		auto pWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(pGLFWWnd));
		pWindow->m_Settings.WindowPosition.X = static_cast<WindowPosition::ValueType>(x);
		pWindow->m_Settings.WindowPosition.Y = static_cast<WindowPosition::ValueType>(y);
		pWindow->OnWindowPositionChanged();
	}

	static void _KeyCallback(GLFWwindow* pGLFWWnd, int key, int scancode, int action, int /*mods*/)
	{
		auto pWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(pGLFWWnd));

		if (action == GLFW_PRESS)
			pWindow->OnKeyDown(key, scancode);
		else if (action == GLFW_RELEASE)
			pWindow->OnKeyUp(key, scancode);
	}

	static void _CharacterCallback(GLFWwindow* pGLFWWnd, unsigned int codepoint)
	{
		auto pWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(pGLFWWnd));
		pWindow->OnCharacter(static_cast<uint32_t>(codepoint));
	}

	static void _CursorPositionCallback(GLFWwindow* pGLFWWnd, double x, double y)
	{
		auto pWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(pGLFWWnd));
		pWindow->OnMouseMoved(static_cast<int64_t>(x), static_cast<int64_t>(y));
	}

	static void _CursorEnterCallback(GLFWwindow* pGLFWWnd, int entered)
	{
		auto pWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(pGLFWWnd));
		pWindow->OnCursorEntered(entered == GLFW_TRUE);
	}

	static void _MouseButtonCallback(GLFWwindow* pGLFWWnd, int button, int action, int /*mods*/)
	{
		auto pWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(pGLFWWnd));
		
		double x, y;
		glfwGetCursorPos(pGLFWWnd, &x, &y);
		
		if(action == GLFW_PRESS)
			pWindow->OnMouseButtonDown(
				static_cast<uint32_t>(button), 
				static_cast<int64_t>(x),
				static_cast<int64_t>(y));
		else if(action == GLFW_RELEASE)
			pWindow->OnMouseButtonUp(
				static_cast<uint32_t>(button),
				static_cast<int64_t>(x),
				static_cast<int64_t>(y));
	}

	static void _ScrollCallback(GLFWwindow* pGLFWWnd, double x, double y)
	{
		auto pWindow = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(pGLFWWnd));
		pWindow->OnMouseScrolled(
			static_cast<int32_t>(x * pWindow->m_Settings.ScrollMultiplier), 
			static_cast<int32_t>(y * pWindow->m_Settings.ScrollMultiplier));
	}

	#pragma endregion

	#pragma region Window/Context Settings

	void ApplyPreCreationWindowSettings(const GLFWvidmode* pVidMode) noexcept
	{
		glfwWindowHint(GLFW_RESIZABLE, m_Settings.IsResizable ? GLFW_TRUE : GLFW_FALSE);
		glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
		glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
		glfwWindowHint(GLFW_FLOATING, m_Settings.IsAlwaysOnTop ? GLFW_TRUE : GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_DECORATED, m_Settings.FullscreenState == eFullscreenState::Window ? GLFW_TRUE : GLFW_FALSE);
		glfwWindowHint(GLFW_MAXIMIZED, m_Settings.WindowState == eWindowState::Maximized ? GLFW_TRUE : GLFW_FALSE);

		glfwWindowHint(GLFW_STEREO, GLFW_FALSE);
		glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
		glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_ROBUSTNESS);
		glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_ANY_RELEASE_BEHAVIOR);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

		glfwWindowHint(GLFW_ALPHA_BITS, m_ContextSettings.AlphaBits);
		glfwWindowHint(GLFW_DEPTH_BITS, m_ContextSettings.DepthBits);
		glfwWindowHint(GLFW_STENCIL_BITS, m_ContextSettings.StencilBits);
		glfwWindowHint(GLFW_SAMPLES, m_ContextSettings.MultiSamples);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_ContextSettings.VersionMajor);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_ContextSettings.VersionMinor);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, m_ContextSettings.VersionMajor >= 3 ? GLFW_TRUE : GLFW_FALSE);

		switch (m_Settings.FullscreenState)
		{
		default:
		case eFullscreenState::Window:
		case eFullscreenState::Fullscreen:
			glfwWindowHint(GLFW_RED_BITS, m_ContextSettings.RedBits);
			glfwWindowHint(GLFW_GREEN_BITS, m_ContextSettings.GreenBits);
			glfwWindowHint(GLFW_BLUE_BITS, m_ContextSettings.BlueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
			break;

		case eFullscreenState::WindowedFullscreen:
			glfwWindowHint(GLFW_RED_BITS, pVidMode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, pVidMode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, pVidMode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, pVidMode->refreshRate);
			break;
		}
	}

	bool ApplyContextSettings() noexcept
	{
		// Set active context
		glfwMakeContextCurrent(m_pWindow);

		// Ensure glew is initialized
		glewExperimental = GL_TRUE;

		if (!GLEWCHECK(glewInit()))
			return false;

		// Apply post-creation context settings
		std::clog << "Renderer " << glGetString(GL_RENDERER) << std::endl;
		std::clog << "OpenGL version supported " << glGetString(GL_VERSION) << std::endl;
		std::clog << std::endl;

		int fbw, fbh;
		glfwGetFramebufferSize(m_pWindow, &fbw, &fbh);
		glViewport(0, 0, fbw, fbh);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		glClearColor
		(
			m_Settings.BackgroundColor.x,
			m_Settings.BackgroundColor.y,
			m_Settings.BackgroundColor.z,
			1.0f
		);

		glfwSwapInterval(m_ContextSettings.WaitForRefresh ? 1 : 0);

		return true;
	}

	void ApplyPostCreationWindowSettings() noexcept
	{
		if (m_Settings.WindowPosition.X != WindowSettings::OSDefaultPosition ||
			m_Settings.WindowPosition.Y != WindowSettings::OSDefaultPosition)
		{
			SetWindowPosition
			(
				(m_Settings.WindowPosition.X != WindowSettings::OSDefaultPosition) ? m_Settings.WindowPosition.X : 0,
				(m_Settings.WindowPosition.Y != WindowSettings::OSDefaultPosition) ? m_Settings.WindowPosition.Y : 0
			);
		}

		SetCursorState(m_Settings.CursorState);

		switch (m_Settings.WindowState)
		{
		default:
		case eWindowState::Hidden:
			break;

		case eWindowState::Minimized:
			glfwShowWindow(m_pWindow);
			glfwIconifyWindow(m_pWindow);
			break;

		case eWindowState::Maximized:
		case eWindowState::Visible:
			glfwShowWindow(m_pWindow);
			break;
		}

		glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, GLFW_FALSE);
		glfwSetInputMode(m_pWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_FALSE);

		// Set window event callbacks
		glfwSetWindowCloseCallback(m_pWindow, &GLFWWindow::_CloseCallback);
		glfwSetFramebufferSizeCallback(m_pWindow, &GLFWWindow::_FramebufferSizeCallback);
		glfwSetWindowPosCallback(m_pWindow, &GLFWWindow::_PositionCallback);
		glfwSetKeyCallback(m_pWindow, &GLFWWindow::_KeyCallback);
		glfwSetCharCallback(m_pWindow, &GLFWWindow::_CharacterCallback);
		glfwSetCursorPosCallback(m_pWindow, &GLFWWindow::_CursorPositionCallback);
		glfwSetCursorEnterCallback(m_pWindow, &GLFWWindow::_CursorEnterCallback);
		glfwSetMouseButtonCallback(m_pWindow, &GLFWWindow::_MouseButtonCallback);
		glfwSetScrollCallback(m_pWindow, &GLFWWindow::_ScrollCallback);
	}

	void PersistCreationSettings() noexcept
	{
		int x, y;

		glfwGetWindowPos(m_pWindow, &x, &y);
		m_Settings.WindowPosition.X = static_cast<WindowPosition::ValueType>(x);
		m_Settings.WindowPosition.Y = static_cast<WindowPosition::ValueType>(y);

		glfwGetFramebufferSize(m_pWindow, &x, &y);
		m_Settings.ClientSize.Width = static_cast<WindowSize::ValueType>(x);
		m_Settings.ClientSize.Height = static_cast<WindowSize::ValueType>(y);
	}

	#pragma endregion

	#pragma region Monitor Selection

	GLFWmonitor* GetWindowMonitor() noexcept
	{
		if (m_Settings.PreferredMonitor == Epic::WindowSettings::PrimaryMonitor)
			return glfwGetPrimaryMonitor();

		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);

		if (m_Settings.PreferredMonitor < 0 || m_Settings.PreferredMonitor >= count)
		{
			m_Settings.PreferredMonitor = Epic::WindowSettings::PrimaryMonitor;
			return glfwGetPrimaryMonitor();
		}

		return monitors[m_Settings.PreferredMonitor];
	}

	#pragma endregion
	
public:
	bool IsClosed() const override
	{
		return (m_pWindow == nullptr);
	}
	
	void Update() override
	{
		glfwPollEvents();
	}

	bool Create() override
	{
		// Ensure the GLFW system is initialized
		if (!Singleton<detail::GLFW>::Instance().Initialize())
			return false;
		
		// Destroy the window if it already exists
		Destroy();

		// Apply pre-creation settings
		GLFWmonitor* pMonitor = GetWindowMonitor();
		const GLFWvidmode* pVidMode = glfwGetVideoMode(pMonitor);
		
		ApplyPreCreationWindowSettings(pVidMode);
		
		// Create the window
		switch (m_Settings.FullscreenState)
		{
		default: 
		case eFullscreenState::Window:
			m_pWindow = glfwCreateWindow(
				m_Settings.ClientSize.Width,
				m_Settings.ClientSize.Height,
				m_Settings.WindowTitle.c_str(),
				nullptr, nullptr);
			break;

		case eFullscreenState::Fullscreen:
			m_pWindow = glfwCreateWindow(
				m_Settings.ClientSize.Width,
				m_Settings.ClientSize.Height,
				m_Settings.WindowTitle.c_str(),
				pMonitor, nullptr);
			break;

		case eFullscreenState::WindowedFullscreen:
			m_pWindow = glfwCreateWindow(
				pVidMode->width,
				pVidMode->height,
				m_Settings.WindowTitle.c_str(),
				pMonitor, nullptr);
			break;
		}

		if (!m_pWindow)
			return false;

		glfwSetWindowUserPointer(m_pWindow, this);

		// Apply post-creation context settings
		if (!ApplyContextSettings())
		{
			Destroy();
			return false;
		}

		// Apply post-creation window settings
		ApplyPostCreationWindowSettings();

		// Retrieve applied window settings
		PersistCreationSettings();

		return true;
	}

	void Destroy() override
	{
		if (m_pWindow)
		{
			glfwDestroyWindow(m_pWindow);
			m_pWindow = nullptr;
		}
	}

	void SetWindowPosition(const Epic::WindowPosition::ValueType& x, const Epic::WindowPosition::ValueType& y) override 
	{
		// Window: Attempts to set the position of the client area to x, y
		// Fullscreen: No effect
		// WindowedFullscreen: No effect

		assert(m_pWindow);
		glfwSetWindowPos(m_pWindow, static_cast<int>(x), static_cast<int>(y));
	}
	
	void SetWindowSize(const Epic::WindowSize::ValueType& cx, const Epic::WindowSize::ValueType& cy) override 
	{
		// Window: Attempts to set the size of the client area to cx, cy
		// Fullscreen: Changes the resolution to the closest match of cx, cy
		// WindowedFullscreen: No effect

		assert(m_pWindow);

		if(m_Settings.FullscreenState != eFullscreenState::WindowedFullscreen)
			glfwSetWindowSize(m_pWindow, static_cast<int>(cx), static_cast<int>(cy));
	}

	void SetWindowState(Epic::eWindowState state) override 
	{
		assert(m_pWindow);

		switch (state)
		{
		default:
		case eWindowState::Visible:
			glfwShowWindow(m_pWindow);
			glfwRestoreWindow(m_pWindow);
			break;

		case eWindowState::Hidden:
			glfwHideWindow(m_pWindow);
			m_Settings.WindowState = eWindowState::Hidden;
			break;

		case eWindowState::Maximized:
			glfwMaximizeWindow(m_pWindow);
			break;

		case eWindowState::Minimized:
			glfwIconifyWindow(m_pWindow);
			break;
		}

		// Because GLFW does not have an associated callback for this feature,
		// the window event must be invoked here. Consequently, if the window
		// state is changed by any other means, the window event will not be called.

		eWindowState curState = m_Settings.WindowState;
		bool isVisible = glfwGetWindowAttrib(m_pWindow, GLFW_VISIBLE) == GLFW_TRUE;
		bool isMaximized = glfwGetWindowAttrib(m_pWindow, GLFW_MAXIMIZED) == GLFW_TRUE;
		bool isMinimized = glfwGetWindowAttrib(m_pWindow, GLFW_ICONIFIED) == GLFW_TRUE;

		if (!isVisible)
			m_Settings.WindowState = eWindowState::Hidden;
		else if (isMaximized)
			m_Settings.WindowState = eWindowState::Maximized;
		else if (isMinimized)
			m_Settings.WindowState = eWindowState::Minimized;
		else
			m_Settings.WindowState = eWindowState::Visible;

		if (curState != m_Settings.WindowState)
			OnWindowStateChanged();
	}

	void SetFullscreenState(Epic::eFullscreenState state) override 
	{
		assert(m_pWindow);

		GLFWmonitor* pMonitor = glfwGetWindowMonitor(m_pWindow);
		if (!pMonitor) pMonitor = GetWindowMonitor();
		const GLFWvidmode* pVidMode = glfwGetVideoMode(pMonitor);

		switch (state)
		{
			default: 
			case eFullscreenState::Window:
				glfwSetWindowMonitor(m_pWindow,
					nullptr,
					m_Settings.WindowPosition.X,
					m_Settings.WindowPosition.Y,
					m_Settings.ClientSize.Width,
					m_Settings.ClientSize.Height,
					0);
			break;
		
			case eFullscreenState::Fullscreen:
				glfwSetWindowMonitor(m_pWindow,
					pMonitor,
					0,
					0,
					m_Settings.ClientSize.Width,
					m_Settings.ClientSize.Height,
					pVidMode->refreshRate);
			break;

			case eFullscreenState::WindowedFullscreen:
				glfwSetWindowMonitor(m_pWindow,
					pMonitor,
					0,
					0,
					pVidMode->width,
					pVidMode->height,
					pVidMode->refreshRate);
			break;
		}

		// Because GLFW does not have an associated callback for this feature,
		// the window event must be invoked here. Consequently, if the window
		// state is changed by any other means, the window event will not be called.

		if (m_Settings.FullscreenState != state)
		{
			m_Settings.FullscreenState = state;
			OnFullscreenStateChanged();
		}
	}
	
	void SetWindowTitle(const Epic::WindowString& title) override 
	{
		// Because GLFW does not have an associated callback for this feature,
		// the window event must be invoked here. Consequently, if the window
		// title is changed by any other means, the window event will not be called.

		assert(m_pWindow);
		
		glfwSetWindowTitle(m_pWindow, title.c_str());
		
		m_Settings.WindowTitle = title;
		OnWindowTitleChanged();
	}
	
	void SetCursorState(Epic::eCursorState state) override 
	{
		assert(m_pWindow);

		switch (state)
		{
		default:
		case eCursorState::Normal:
			glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			break;

		case eCursorState::Hidden:
			glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			break;

		case eCursorState::Virtual:
			glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			break;
		}
	}

	void SetBackgroundColor(const float& r, const float& g, const float& b) override
	{
		assert(m_pWindow);
		glClearColor(r, g, b, 1.0f);
		OnBackgroundColorChanged();
	}

public:
	const WindowSize GetFramebufferSize() const noexcept
	{
		assert(m_pWindow);

		int width, height;
		glfwGetFramebufferSize(m_pWindow, &width, &height);

		return{ static_cast<WindowSize::ValueType>(width), 
				static_cast<WindowSize::ValueType>(height) };
	}

public:
	inline void Clear() noexcept
	{
		assert(m_pWindow);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	inline void Display() noexcept
	{
		assert(m_pWindow);
		glfwSwapBuffers(m_pWindow);
	}

	inline void Close()
	{
		assert(m_pWindow);
		
		if (OnCloseWindow())
		{
			Destroy();
			OnDestroyed();
		}
		else
			glfwSetWindowShouldClose(m_pWindow, GLFW_FALSE);
	}

	inline void OnCursorEntered(bool entered)
	{
		this->CursorEntered(*this, entered);
	}

public:
	typedef Epic::Event<void(GLFWWindow &, bool)> CursorEnteredDelegate;

public:
	CursorEnteredDelegate CursorEntered;
};

