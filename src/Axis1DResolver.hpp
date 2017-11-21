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
	class Axis1DResolver;
}

//////////////////////////////////////////////////////////////////////////////

// Axis1DResolver
class Epic::Axis1DResolver : public Epic::InputResolver
{
public:
	using Type = Epic::Axis1DResolver;
	using Base = Epic::InputResolver;

private:
	InputDataID m_DataID;
	Epic::StringHash m_DeviceName;

public:
	Axis1DResolver() noexcept = delete;

	Axis1DResolver(InputDataID dataID, Epic::StringHash device)
		: m_DataID{ dataID }, m_DeviceName{ device }
	{ }

	virtual ~Axis1DResolver() noexcept = default;

public:
	inline InputDataID GetAxisID() const noexcept
	{
		return m_DataID;
	}

	inline Epic::StringHash GetDeviceName() const noexcept
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
		if (data.DataType != Epic::eInputDataType::Axis1D)
			return false;

		// Verify axis
		if (data.Data.Axis1D.Axis0.AxisID != m_DataID)
			return false;

		// This event can be resolved
		return true;
	}
};
