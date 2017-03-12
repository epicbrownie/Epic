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

#include <Epic/InputResolver.hpp>
#include <Epic/StringHash.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class ButtonResolver;
}

//////////////////////////////////////////////////////////////////////////////

// ButtonResolver
class Epic::ButtonResolver : public Epic::InputResolver
{
public:
	using Type = Epic::ButtonResolver;
	using Base = Epic::InputResolver;

private:
	InputDataID m_DataID;
	eInputButtonState m_State;
	Epic::StringHash m_DeviceName;

public:
	ButtonResolver() noexcept = delete;

	ButtonResolver(InputDataID dataID, eInputButtonState resolveOn, const Epic::StringHash& device)
		: m_DataID{ dataID }, m_State{ resolveOn }, m_DeviceName{ device }
	{ }

	virtual ~ButtonResolver() noexcept = default;

public:
	inline const InputDataID GetButtonID() const noexcept
	{
		return m_DataID;
	}

	inline const eInputButtonState GetResolveState() const noexcept
	{
		return m_State;
	}

	inline const Epic::StringHash& GetDeviceName() const noexcept
	{
		return m_DeviceName;
	}

public:
	bool Resolve(const InputData& data) const noexcept
	{
		// Verify device
		if (data.Device != m_DeviceName)
			return false;

		// Verify data type
		if (data.DataType != Epic::eInputDataType::Button)
			return false;

		// Verify button
		if (data.Data.Button.ButtonID != m_DataID)
			return false;

		// Verify state
		if (m_State != eInputButtonState::Both &&
			m_State != data.Data.Button.State)
			return false;

		// This event can be resolved
		return true;
	}
};
