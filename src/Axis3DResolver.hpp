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
	class Axis3DResolver;
}

//////////////////////////////////////////////////////////////////////////////

// Axis3DResolver
class Epic::Axis3DResolver : public Epic::InputResolver
{
public:
	using Type = Epic::Axis3DResolver;
	using Base = Epic::InputResolver;

private:
	InputDataID m_Data1ID, m_Data2ID, m_Data3ID;
	Epic::StringHash m_DeviceName;

public:
	Axis3DResolver() noexcept = delete;

	Axis3DResolver(InputDataID axis0ID, InputDataID axis1ID, InputDataID axis2ID, Epic::StringHash device)
		: m_Data1ID{ axis0ID }, m_Data2ID{ axis1ID }, m_Data3ID{ axis2ID }, m_DeviceName { device }
	{ }

	virtual ~Axis3DResolver() noexcept = default;

public:
	inline InputDataID GetAxis0ID() const noexcept
	{
		return m_Data1ID;
	}

	inline InputDataID GetAxis1ID() const noexcept
	{
		return m_Data2ID;
	}
	
	inline InputDataID GetAxis2ID() const noexcept
	{
		return m_Data3ID;
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
		if (data.DataType != Epic::eInputDataType::Axis3D)
			return false;

		// Verify axes
		if (data.Data.Axis3D.Axis0.AxisID != m_Data1ID ||
			data.Data.Axis3D.Axis1.AxisID != m_Data2ID ||
			data.Data.Axis3D.Axis2.AxisID != m_Data3ID)
			return false;

		// This event can be resolved
		return true;
	}
};
