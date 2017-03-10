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

#include <Epic/Core/detail/WindowFwd.hpp>
#include <Epic/Core/Event.hpp>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////	

// Window
class Epic::Window
{
public:
	using Type = Epic::Window;

protected:
	Window(const WindowSettings& properties) noexcept 
		: m_Settings(properties)
	{ }

public:
	virtual ~Window() = default;

public:
	inline const Epic::WindowPosition& GetWindowPosition() const noexcept
	{
		return m_Settings.WindowPosition;
	}

	inline const Epic::WindowSize& GetClientSize() const noexcept
	{
		return m_Settings.ClientSize;
	}

	inline const Epic::WindowSize& GetWindowSize() const noexcept
	{
		return m_Settings.ClientSize;
	}

	inline Epic::eWindowState GetWindowState() const noexcept
	{
		return m_Settings.WindowState;
	}

	inline Epic::eFullscreenState GetFullscreenState() const noexcept
	{
		return m_Settings.FullscreenState;
	}

	inline Epic::eCursorState GetCursorState() const noexcept
	{
		return m_Settings.CursorState;
	}

	inline const WindowString& GetWindowTitle() const noexcept
	{
		return m_Settings.WindowTitle;
	}

public:
	inline void SetWindowPosition(const Epic::WindowPosition& pos)
	{
		SetWindowPosition(pos.X, pos.Y);
	}

	inline void SetWindowSize(const Epic::WindowSize& size)
	{
		SetWindowSize(size.Width, size.Height);
	}

public:
	virtual void Update() { };

	virtual bool Create() = 0;
	virtual void Destroy() = 0;
	virtual bool IsClosed() const = 0;

	virtual void SetWindowPosition(const Epic::WindowPosition::ValueType& x, const Epic::WindowPosition::ValueType& y) = 0;
	virtual void SetWindowSize(const Epic::WindowSize::ValueType& cx, const Epic::WindowSize::ValueType& cy) = 0;
	virtual void SetWindowState(Epic::eWindowState state) = 0;
	virtual void SetFullscreenState(Epic::eFullscreenState state) = 0;
	virtual void SetCursorState(Epic::eCursorState state) = 0;
	virtual void SetWindowTitle(const WindowString& title) = 0;

protected:
	inline bool OnCloseWindow() noexcept
	{
		// The window will close if any of the handlers return true
		// Or if no handlers have been registered
		
		if (!Closing || Closing.InvokeUntil(true, *this))
			return true;

		return false;
	}

	inline void OnDestroyed() noexcept
	{
		this->Destroyed(*this);
	}

	inline void OnKeyDown(int32_t key, int32_t scancode)
	{
		this->KeyDown(*this, key, scancode);
	}

	inline void OnKeyUp(int32_t key, int32_t scancode)
	{
		this->KeyUp(*this, key, scancode);
	}

	inline void OnCharacter(uint32_t code)
	{
		this->Character(*this, code);
	}

	inline void OnMouseButtonDown(uint32_t button, int64_t x, int64_t y)
	{
		this->MouseButtonDown(*this, button, x, y);
	}

	inline void OnMouseButtonUp(uint32_t button, int64_t x, int64_t y)
	{
		this->MouseButtonUp(*this, button, x, y);
	}

	inline void OnMouseMoved(int64_t x, int64_t y)
	{
		this->MouseMoved(*this, x, y);
	}

	inline void OnMouseScrolled(int32_t x, int32_t y)
	{
		this->MouseScrolled(*this, x, y);
	}

	inline void OnWindowPositionChanged()
	{
		this->WindowPositionChanged(*this, m_Settings);
	}

	inline void OnWindowSizeChanged()
	{
		this->WindowSizeChanged(*this, m_Settings);
	}

	inline void OnWindowStateChanged()
	{
		this->WindowStateChanged(*this, m_Settings);
	}

	inline void OnWindowTitleChanged()
	{
		this->WindowTitleChanged(*this, m_Settings);
	}

	inline void OnFullscreenStateChanged()
	{
		this->FullscreenStateChanged(*this, m_Settings);
	}

public:
	typedef Epic::Event<void(Window &, const WindowSettings &)>		WindowSettingChangedDelegate;
	typedef Epic::Event<bool(Window &)>								WindowClosingDelegate;
	typedef Epic::Event<void(Window &)>								WindowDestroyedDelegate;
	typedef Epic::Event<void(Window &, int32_t, int32_t)>			KeyStateChangedDelegate;
	typedef Epic::Event<void(Window &, uint32_t)>					CharacterDelegate;
	typedef Epic::Event<void(Window &, uint32_t, int64_t, int64_t)>	MouseButtonDelegate;
	typedef Epic::Event<void(Window &, int64_t, int64_t)>			MouseMovedDelegate;
	typedef Epic::Event<void(Window &, int32_t, int32_t)>			MouseScrolledDelegate;

public:
	WindowClosingDelegate			Closing;
	WindowDestroyedDelegate			Destroyed;
	KeyStateChangedDelegate			KeyDown;
	KeyStateChangedDelegate			KeyUp;
	CharacterDelegate				Character;
	MouseButtonDelegate				MouseButtonDown;
	MouseButtonDelegate				MouseButtonUp;
	MouseScrolledDelegate			MouseScrolled;
	MouseMovedDelegate				MouseMoved;
	WindowSettingChangedDelegate	WindowPositionChanged;
	WindowSettingChangedDelegate	WindowSizeChanged;
	WindowSettingChangedDelegate	WindowStateChanged;
	WindowSettingChangedDelegate	WindowTitleChanged;
	WindowSettingChangedDelegate	FullscreenStateChanged;

protected:
	WindowSettings m_Settings;
};
