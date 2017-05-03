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

#include <Epic/STL/String.hpp>
#include <Epic/Math/Vector.hpp>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	using WindowString = Epic::STLString<char>; // UTF-8

	struct WindowPosition;
	struct WindowSize;
	struct WindowSettings;

	enum class eWindowState : uint32_t
	{
		Unknown = 1,
		Hidden = 2,
		Visible = 4,
		Maximized = 8,
		Minimized = 16
	};

	enum class eFullscreenState : uint32_t
	{
		Window = 1,
		Fullscreen = 2,
		WindowedFullscreen = 3
	};

	enum class eCursorState : uint8_t
	{
		Normal = 1,
		Hidden = 2,
		Virtual = 3
	};
}

//////////////////////////////////////////////////////////////////////////////

// WindowPosition
struct Epic::WindowPosition
{
	using ValueType = int32_t;
	ValueType X, Y;
};

//////////////////////////////////////////////////////////////////////////////

// WindowSize
struct Epic::WindowSize
{
	using ValueType = uint16_t;
	ValueType Width, Height;
};

//////////////////////////////////////////////////////////////////////////////

// WindowSettings
struct Epic::WindowSettings
{
	static constexpr WindowPosition::ValueType OSDefaultPosition = -1;
	static constexpr int PrimaryMonitor = -1;

	WindowSettings() noexcept
		: WindowPosition{ OSDefaultPosition, OSDefaultPosition },
		  ClientSize{ 0, 0 },
		  WindowState{ Epic::eWindowState::Visible },
		  FullscreenState{ Epic::eFullscreenState::Window },
		  CursorState{ Epic::eCursorState::Normal },
		  ScrollMultiplier{ 40.0 },
		  IsResizable{ true },
		  IsAlwaysOnTop{ false },
		  PreferredMonitor{ PrimaryMonitor },
		  BackgroundColor{ 0.0f }
	{ }

	Epic::WindowPosition	WindowPosition;		// Default: OSDefaultPosition, OSDefaultPosition
	Epic::WindowSize		ClientSize;			// Default: 0, 0
	Epic::eWindowState		WindowState;		// Default: Visible
	Epic::eFullscreenState	FullscreenState;	// Default: Window
	Epic::eCursorState		CursorState;		// Default: Normal
	WindowString			WindowTitle;		// Default: ""
	double					ScrollMultiplier;	// Default: 40.0
	bool					IsResizable;		// Default: true
	bool					IsAlwaysOnTop;		// Default: false
	int						PreferredMonitor;	// Default: PrimaryMonitor
	Epic::Color3f			BackgroundColor;	// Default: (0, 0, 0)
};

//////////////////////////////////////////////////////////////////////////////	

namespace Epic
{
	inline bool operator == (const WindowSize& a, const WindowSize& b)
	{
		return a.Width == b.Width && a.Height == b.Height;
	}

	inline bool operator != (const WindowSize& a, const WindowSize& b)
	{
		return a.Width != b.Width || a.Height != b.Height;
	}
}
