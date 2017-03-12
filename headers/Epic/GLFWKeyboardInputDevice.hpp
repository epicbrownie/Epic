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
#include <Epic/InputDevice.hpp>
#include <Epic/InputResolver.hpp>
#include <Epic/AlphaResolver.hpp>
#include <Epic/AlphaNumResolver.hpp>
#include <Epic/ButtonResolver.hpp>
#include <Epic/HexResolver.hpp>
#include <Epic/NumericalResolver.hpp>
#include <Epic/TextResolver.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class GLFWKeyboardInputDevice;
}

//////////////////////////////////////////////////////////////////////////////

// GLFWKeyboardInputDevice
class Epic::GLFWKeyboardInputDevice : public Epic::InputDevice
{
public:
	using Type = Epic::GLFWKeyboardInputDevice;
	using Base = Epic::InputDevice;

public:
	using InputResolverPtr = Base::InputResolverPtr;

private:
	Epic::GLFWWindow* m_pWindow;

public:
	GLFWKeyboardInputDevice(const Epic::StringHash& deviceName, Epic::GLFWWindow* pWindow) noexcept
		: Base{ deviceName }, m_pWindow{ pWindow }
	{
		// Connect to window events
		if (m_pWindow)
		{
			m_pWindow->KeyDown.Connect(this, &Type::OnKeyDown);
			m_pWindow->KeyUp.Connect(this, &Type::OnKeyUp);
			m_pWindow->Character.Connect(this, &Type::OnCharacter);
		}
	}

	~GLFWKeyboardInputDevice() noexcept
	{
		m_pWindow->Character.DisconnectAll(this);
		m_pWindow->KeyUp.DisconnectAll(this);
		m_pWindow->KeyDown.DisconnectAll(this);
	}

public:
	const uint64_t GetDeviceAttribute(const Epic::StringHash& attrib) const noexcept override
	{
		if (attrib == Epic::Hash("Type"))
			return static_cast<uint64_t>(Epic::Hash("Keyboard").Value());

		return Base::GetDeviceAttribute(attrib);
	}

private:
	void OnKeyDown(Epic::Window&, int32_t key, int32_t scanCode)
	{
		OnInput(Epic::InputButtonData{ static_cast<InputDataID>(key), eInputButtonState::Down });
	}

	void OnKeyUp(Epic::Window&, int32_t key, int32_t scanCode)
	{
		OnInput(Epic::InputButtonData{ static_cast<InputDataID>(key), eInputButtonState::Up });
	}

	void OnCharacter(Epic::Window&, uint32_t code)
	{
		OnInput(Epic::InputButtonData{ code | detail::InputTextCode, eInputButtonState::Down });
		OnInput(Epic::InputButtonData{ code | detail::InputTextCode, eInputButtonState::Up });
	}

public:
	InputResolverPtr CreateResolverFor(const InputData& data) const override
	{
		if (data.DataType != Epic::eInputDataType::Button)
			return nullptr;

		return Epic::MakeImpl<InputResolver, ButtonResolver>
			(data.Data.Button.ButtonID, data.Data.Button.State, GetDeviceName());
	}

public:
	const char* GetKeyName(InputDataID key) const noexcept
	{
		return glfwGetKeyName(static_cast<int>(key), 0);
	}

	inline InputResolverPtr CreateKeyResolver(Epic::Keys key, eInputButtonState resolveOn = eInputButtonState::Down) const noexcept
	{
		return Epic::MakeImpl<InputResolver, ButtonResolver>
			(static_cast<InputDataID>(key), resolveOn, GetDeviceName());
	}

	inline InputResolverPtr CreateAlphaResolver(eInputButtonState resolveOn = eInputButtonState::Down) const noexcept
	{
		return Epic::MakeImpl<InputResolver, AlphaResolver>(resolveOn, GetDeviceName());
	}

	inline InputResolverPtr CreateAlphaNumericalResolver(eInputButtonState resolveOn = eInputButtonState::Down) const noexcept
	{
		return Epic::MakeImpl<InputResolver, AlphaNumResolver>(resolveOn, GetDeviceName());
	}

	inline InputResolverPtr CreateNumericalResolver(eInputButtonState resolveOn = eInputButtonState::Down) const noexcept
	{
		return Epic::MakeImpl<InputResolver, NumericalResolver>(resolveOn, GetDeviceName());
	}
	
	inline InputResolverPtr CreateHexadecimalResolver(eInputButtonState resolveOn = eInputButtonState::Down) const noexcept
	{
		return Epic::MakeImpl<InputResolver, HexResolver>(resolveOn, GetDeviceName());
	}

	inline InputResolverPtr CreateTextResolver(eInputButtonState resolveOn = eInputButtonState::Down) const noexcept
	{
		return Epic::MakeImpl<InputResolver, TextResolver>(resolveOn, GetDeviceName());
	}
};
