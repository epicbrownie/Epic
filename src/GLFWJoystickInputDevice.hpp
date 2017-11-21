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

#include <Epic/detail/GLFWInclude.hpp>
#include <Epic/Event.hpp>
#include <Epic/InputDevice.hpp>
#include <Epic/InputResolver.hpp>
#include <Epic/Axis1DResolver.hpp>
#include <Epic/ButtonResolver.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <array>
#include <bitset>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class GLFWJoystickInputDevice;
}

//////////////////////////////////////////////////////////////////////////////

// GLFWJoystickInputDevice
class Epic::GLFWJoystickInputDevice : public Epic::InputDevice
{
public:
	using Type = Epic::GLFWJoystickInputDevice;
	using Base = Epic::InputDevice;

public:
	using InputResolverPtr = Base::InputResolverPtr;

public:
	static constexpr int MaxJoystickID = GLFW_JOYSTICK_LAST;

private:
	static constexpr size_t MaxButtons = 128;
	static constexpr size_t MaxAxes = 16;

private:
	int m_JoystickID;
	int m_ButtonCount, m_AxisCount;
	std::bitset<MaxButtons> m_ButtonData;
	std::array<float, MaxAxes> m_AxisData;

public:
	GLFWJoystickInputDevice(Epic::StringHash deviceName, int joystickID) noexcept
		: Base{ deviceName }
	{
		m_JoystickID = (joystickID <= MaxJoystickID) ? joystickID : -1;
		m_ButtonData.reset();
		m_AxisData.fill(0.0f);

		if (glfwJoystickPresent(m_JoystickID) == GLFW_FALSE)
		{
			m_ButtonCount = 0;
			m_AxisCount = 0;
		}
		else
		{
			glfwGetJoystickButtons(m_JoystickID, &m_ButtonCount);
			glfwGetJoystickAxes(m_JoystickID, &m_AxisCount);
		}
	}

	~GLFWJoystickInputDevice() noexcept { }

public:
	uint64_t GetDeviceAttribute(Epic::StringHash attrib) const noexcept override
	{
		switch (attrib)
		{
		case Epic::Hash("Type").Value():
			return static_cast<uint64_t>(Epic::Hash("Joystick"));

		case Epic::Hash("JoystickID").Value():
			return static_cast<uint64_t>(m_JoystickID);

		default:
			return Base::GetDeviceAttribute(attrib);
		}
	}

public:
	const char* GetJoystickName() const noexcept
	{
		return glfwGetJoystickName(m_JoystickID);
	}

	void Update() noexcept override
	{
		int buttonCount, axisCount;
		const auto* pButtons = glfwGetJoystickButtons(m_JoystickID, &buttonCount);
		const float* pAxes = glfwGetJoystickAxes(m_JoystickID, &axisCount);
	
		// If the button count or axis count have changed, then the joystick has changed.
		if (buttonCount != m_ButtonCount || axisCount != m_AxisCount)
		{
			if (glfwJoystickPresent(m_JoystickID) == GLFW_TRUE)
				this->JoystickConnected(this);
			else
				this->JoystickDisconnected(this);

			m_ButtonCount = buttonCount;
			m_AxisCount = axisCount;

			m_ButtonData.reset();
			m_AxisData.fill(0.0f);
		}

		// Look for button state changes
		for (int i = 0; i < m_ButtonCount; ++i)
		{
			bool state = pButtons[i] == GLFW_PRESS;
			if (state != m_ButtonData.test(i))
			{
				m_ButtonData.set(i, state);

				OnInput(Epic::InputButtonData
				{ 
					static_cast<InputDataID>(i), 
					state ? eInputButtonState::Down : eInputButtonState::Up 
				});
			}
		}

		// Look for axis state changes
		for (int i = 0; i < m_AxisCount; ++i)
		{
			if (pAxes[i] != m_AxisData[i])
			{
				m_AxisData[i] = pAxes[i];

				OnInput(Epic::InputAxisData
				{
					static_cast<InputDataID>(i),
					1,
					static_cast<double>(pAxes[i])
				});
			}
		}
	}

public:
	InputResolverPtr CreateResolverFor(const InputData& data) const override
	{
		switch (data.DataType)
		{
		case eInputDataType::Button:
			return CreateButtonResolver(static_cast<size_t>(data.Data.Button.ButtonID), data.Data.Button.State);

		case eInputDataType::Axis1D:
			return CreateAxisResolver(static_cast<size_t>(data.Data.Axis1D.Axis0.AxisID));

		default:
			return nullptr;
		}
	}

	inline InputResolverPtr CreateButtonResolver(size_t btn, eInputButtonState resolveOn = eInputButtonState::Down) const noexcept
	{
		return Epic::MakeImpl<InputResolver, ButtonResolver>
			(static_cast<InputDataID>(btn), resolveOn, GetDeviceName());
	}

	inline InputResolverPtr CreateAxisResolver(size_t axis) const noexcept
	{
		return Epic::MakeImpl<InputResolver, Axis1DResolver>
			(static_cast<InputDataID>(axis), GetDeviceName());
	}

private:
	using ConnectionDelegate = Epic::Event<void(Epic::GLFWJoystickInputDevice*)>;

public:
	ConnectionDelegate JoystickConnected;
	ConnectionDelegate JoystickDisconnected;
};
