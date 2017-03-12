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
#include <cwctype>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class NumericalResolver;
}

//////////////////////////////////////////////////////////////////////////////

// NumericalResolver
class Epic::NumericalResolver : public Epic::InputResolver
{
public:
	using Type = Epic::NumericalResolver;
	using Base = Epic::InputResolver;

private:
	eInputButtonState m_State;
	Epic::StringHash m_DeviceName;

public:
	NumericalResolver() noexcept = delete;

	NumericalResolver(eInputButtonState resolveOn, const Epic::StringHash& device)
		: m_State{ resolveOn }, m_DeviceName{ device }
	{ }

	virtual ~NumericalResolver() noexcept = default;

public:
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

		// Verify data is numerical [0123456789]
		if ((data.Data.Button.ButtonID & detail::InputTextCodeMask) != detail::InputTextCode)
			return false;

		if (!std::iswdigit(static_cast<wchar_t>(data.Data.Button.ButtonID & ~detail::InputTextCodeMask)))
			return false;

		// Verify state
		if (m_State != eInputButtonState::Both &&
			m_State != data.Data.Button.State)
			return false;

		// This event can be resolved
		return true;
	}
};
