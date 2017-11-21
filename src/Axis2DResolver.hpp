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
	class Axis2DResolver;
}

//////////////////////////////////////////////////////////////////////////////

// Axis2DResolver
class Epic::Axis2DResolver : public Epic::InputResolver
{
public:
	using Type = Epic::Axis2DResolver;
	using Base = Epic::InputResolver;

private:
	InputDataID m_Data1ID, m_Data2ID;
	Epic::StringHash m_DeviceName;

public:
	Axis2DResolver() noexcept = delete;

	Axis2DResolver(InputDataID axis0ID, InputDataID axis1ID, Epic::StringHash device)
		: m_Data1ID{ axis0ID }, m_Data2ID{ axis1ID }, m_DeviceName{ device }
	{ }

	virtual ~Axis2DResolver() noexcept = default;

public:
	inline InputDataID GetAxis0ID() const noexcept
	{
		return m_Data1ID;
	}

	inline InputDataID GetAxis1ID() const noexcept
	{
		return m_Data2ID;
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
		if (data.DataType != Epic::eInputDataType::Axis2D)
			return false;

		// Verify axes
		if (data.Data.Axis2D.Axis0.AxisID != m_Data1ID ||
			data.Data.Axis2D.Axis1.AxisID != m_Data2ID)
			return false;

		// This event can be resolved
		return true;
	}
};
