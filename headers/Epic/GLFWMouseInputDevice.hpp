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

#include <Epic/Event.hpp>
#include <Epic/GLFWWindow.hpp>
#include <Epic/InputResolver.hpp>
#include <Epic/InputDevice.hpp>
#include <Epic/Axis1DResolver.hpp>
#include <Epic/Axis2DResolver.hpp>
#include <Epic/ButtonResolver.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class GLFWMouseInputDevice;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		static constexpr Epic::InputDataID GLFWButtonMask = 0x1000;
		static constexpr Epic::InputDataID GLFWMotionMask = 0x2000;
		static constexpr Epic::InputDataID GLFWScrollMask = 0x3000;
	}

	enum class eGLFWMouseButton
	{
		Button1 = detail::GLFWButtonMask | GLFW_MOUSE_BUTTON_1,
		Button2 = detail::GLFWButtonMask | GLFW_MOUSE_BUTTON_2,
		Button3 = detail::GLFWButtonMask | GLFW_MOUSE_BUTTON_3,
		Button4 = detail::GLFWButtonMask | GLFW_MOUSE_BUTTON_4,
		Button5 = detail::GLFWButtonMask | GLFW_MOUSE_BUTTON_5,
		Button6 = detail::GLFWButtonMask | GLFW_MOUSE_BUTTON_6,
		Button7 = detail::GLFWButtonMask | GLFW_MOUSE_BUTTON_7,
		Button8 = detail::GLFWButtonMask | GLFW_MOUSE_BUTTON_8,
		Left = detail::GLFWButtonMask | GLFW_MOUSE_BUTTON_LEFT,
		Right = detail::GLFWButtonMask | GLFW_MOUSE_BUTTON_RIGHT,
		Middle = detail::GLFWButtonMask | GLFW_MOUSE_BUTTON_MIDDLE
	};

	enum class eGLFWMotionAxis
	{
		X = detail::GLFWMotionMask | 0x1,
		Y = detail::GLFWMotionMask | 0x2
	};

	enum class eGLFWScrollAxis
	{
		X = detail::GLFWScrollMask | 0x1,
		Y = detail::GLFWScrollMask | 0x2
	};
}

//////////////////////////////////////////////////////////////////////////////

// GLFWMouseInputDevice
class Epic::GLFWMouseInputDevice : public Epic::InputDevice
{
public:
	using Type = Epic::GLFWMouseInputDevice;
	using Base = Epic::InputDevice;

public:
	using InputResolverPtr = Base::InputResolverPtr;

private:
	Epic::GLFWWindow* m_pWindow;

public:
	GLFWMouseInputDevice(const Epic::StringHash& deviceName, Epic::GLFWWindow* pWindow) noexcept
		: Base{ deviceName }, m_pWindow{ pWindow }
	{
		// Connect to window events
		if (m_pWindow)
		{
			m_pWindow->MouseButtonDown.Connect(this, &Type::OnMouseButtonDown);
			m_pWindow->MouseButtonUp.Connect(this, &Type::OnMouseButtonUp);
			m_pWindow->MouseMoved.Connect(this, &Type::OnMouseMoved);
			m_pWindow->MouseScrolled.Connect(this, &Type::OnMouseScrolled);
		}
	}

	~GLFWMouseInputDevice() noexcept
	{
		m_pWindow->MouseScrolled.DisconnectAll(this);
		m_pWindow->MouseMoved.DisconnectAll(this);
		m_pWindow->MouseButtonUp.DisconnectAll(this);
		m_pWindow->MouseButtonDown.DisconnectAll(this);
	}

public:
	const uint64_t GetDeviceAttribute(const Epic::StringHash& attrib) const noexcept override
	{
		if (attrib == Epic::Hash("Type"))
			return static_cast<uint64_t>(Epic::Hash("Mouse").Value());

		return Base::GetDeviceAttribute(attrib);
	}

private:
	void OnMouseButtonDown(Epic::Window&, uint32_t btn, int64_t, int64_t)
	{
		OnInput(Epic::InputButtonData{ detail::GLFWButtonMask | btn, eInputButtonState::Down });
	}

	void OnMouseButtonUp(Epic::Window&, uint32_t btn, int64_t, int64_t)
	{
		OnInput(Epic::InputButtonData{ detail::GLFWButtonMask | btn, eInputButtonState::Up });
	}

	void OnMouseMoved(Epic::Window&, int64_t x, int64_t y)
	{
		OnInput(Epic::InputAxisData{ static_cast<InputDataID>(eGLFWMotionAxis::X), x, 1.0 });
		OnInput(Epic::InputAxisData{ static_cast<InputDataID>(eGLFWMotionAxis::Y), y, 1.0 });
		OnInput(Epic::InputAxisData{ static_cast<InputDataID>(eGLFWMotionAxis::X), x, 1.0 },
				Epic::InputAxisData{ static_cast<InputDataID>(eGLFWMotionAxis::Y), y, 1.0 });
	}

	void OnMouseScrolled(Epic::Window&, int32_t x, int32_t y)
	{
		OnInput(Epic::InputAxisData{ static_cast<InputDataID>(eGLFWScrollAxis::X), x, 1.0 });
		OnInput(Epic::InputAxisData{ static_cast<InputDataID>(eGLFWScrollAxis::Y), y, 1.0 });
		OnInput(Epic::InputAxisData{ static_cast<InputDataID>(eGLFWScrollAxis::X), x, 1.0 },
				Epic::InputAxisData{ static_cast<InputDataID>(eGLFWScrollAxis::Y), y, 1.0 });
	}

public:
	InputResolverPtr CreateResolverFor(const InputData& data) const override
	{
		switch (data.DataType)
		{
			case eInputDataType::Button:
				switch (data.Data.Button.ButtonID)
				{
					case (InputDataID)eGLFWMouseButton::Button1:
					case (InputDataID)eGLFWMouseButton::Button2:
					case (InputDataID)eGLFWMouseButton::Button3:
					case (InputDataID)eGLFWMouseButton::Button4:
					case (InputDataID)eGLFWMouseButton::Button5:
					case (InputDataID)eGLFWMouseButton::Button6:
					case (InputDataID)eGLFWMouseButton::Button7:
					case (InputDataID)eGLFWMouseButton::Button8:
						return CreateButtonResolver(static_cast<eGLFWMouseButton>(data.Data.Button.ButtonID), data.Data.Button.State);

					default: break;
				}
			break;

			case eInputDataType::Axis1D:
				switch (data.Data.Axis1D.Axis0.AxisID)
				{
					case (InputDataID)eGLFWMotionAxis::X: return CreateXMotionResolver();
					case (InputDataID)eGLFWMotionAxis::Y: return CreateYMotionResolver();
					case (InputDataID)eGLFWScrollAxis::X: return CreateXScrollResolver();
					case (InputDataID)eGLFWScrollAxis::Y: return CreateYScrollResolver();
					default: break;
				}
			break;

			case eInputDataType::Axis2D:
				if (data.Data.Axis2D.Axis0.AxisID == (InputDataID)eGLFWMotionAxis::X &&
					data.Data.Axis2D.Axis1.AxisID == (InputDataID)eGLFWMotionAxis::Y)
					return CreateMotionResolver();
				else if (data.Data.Axis2D.Axis0.AxisID == (InputDataID)eGLFWScrollAxis::X &&
						 data.Data.Axis2D.Axis1.AxisID == (InputDataID)eGLFWScrollAxis::Y)
					return CreateScrollResolver();
			break;

			default: break;
		}
		
		return nullptr;
	}

public:
	inline InputResolverPtr CreateButtonResolver(eGLFWMouseButton btn, eInputButtonState resolveOn = eInputButtonState::Down) const noexcept
	{
		return Epic::MakeImpl<InputResolver, ButtonResolver>
			(static_cast<InputDataID>(btn), resolveOn, GetDeviceName());
	}

	inline InputResolverPtr CreateMotionResolver() const noexcept
	{
		return Epic::MakeImpl<InputResolver, Axis2DResolver>
			(static_cast<InputDataID>(eGLFWMotionAxis::X), 
			 static_cast<InputDataID>(eGLFWMotionAxis::Y), GetDeviceName());
	}

	inline InputResolverPtr CreateXMotionResolver() const noexcept
	{
		return Epic::MakeImpl<InputResolver, Axis1DResolver>
			(static_cast<InputDataID>(eGLFWMotionAxis::X), GetDeviceName());
	}
	
	inline InputResolverPtr CreateYMotionResolver() const noexcept
	{
		return Epic::MakeImpl<InputResolver, Axis1DResolver>
			(static_cast<InputDataID>(eGLFWMotionAxis::Y), GetDeviceName());
	}

	inline InputResolverPtr CreateScrollResolver() const noexcept
	{
		return Epic::MakeImpl<InputResolver, Axis2DResolver>
			(static_cast<InputDataID>(eGLFWScrollAxis::X), 
			 static_cast<InputDataID>(eGLFWScrollAxis::Y), GetDeviceName());
	}

	inline InputResolverPtr CreateXScrollResolver() const noexcept
	{
		return Epic::MakeImpl<InputResolver, Axis1DResolver>
			(static_cast<InputDataID>(eGLFWScrollAxis::X), GetDeviceName());
	}
	
	inline InputResolverPtr CreateYScrollResolver() const noexcept
	{
		return Epic::MakeImpl<InputResolver, Axis1DResolver>
			(static_cast<InputDataID>(eGLFWScrollAxis::Y), GetDeviceName());
	}
};
