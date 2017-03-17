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

#include <Epic/Clock.hpp>
#include <Epic/Event.hpp>
#include <Epic/InputData.hpp>
#include <Epic/InputResolver.hpp>
#include <Epic/DeviceResolver.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class InputDevice;
}

//////////////////////////////////////////////////////////////////////////////

// InputDevice
class Epic::InputDevice
{
public:
	using Type = Epic::InputDevice;
	
public:
	using InputResolverPtr = Epic::ImplPtr<Epic::InputResolver>;

private:
	Epic::StringHash m_DeviceName;

public:
	explicit InputDevice(const Epic::StringHash& deviceName) noexcept
		: m_DeviceName(deviceName)
	{ }

	virtual ~InputDevice() noexcept = default;

public:
	inline const Epic::StringHash& GetDeviceName() const noexcept
	{
		return m_DeviceName;
	}

	virtual const uint64_t GetDeviceAttribute(const Epic::StringHash& attrib) const noexcept = 0
	{
		if (attrib == Epic::Hash("Name"))
			return static_cast<uint64_t>(m_DeviceName);

		return 0;
	}

	virtual void Update() noexcept { };

public:
	virtual InputResolverPtr CreateResolverFor(const InputData& data) const = 0;

public:
	inline InputResolverPtr CreateDeviceResolver() const
	{
		return Epic::MakeImpl<InputResolver, DeviceResolver>(m_DeviceName);
	}

protected:
	void OnInput(Epic::InputButtonData&& buttonData) noexcept
	{
		Epic::InputData data
		{
			Epic::eInputDataType::Button,
			m_DeviceName,
			Epic::StandardClock.Now(),
			{ std::move(buttonData) }
		};

		this->Input(data);
	}

	void OnInput(Epic::InputAxisData&& axisData0) noexcept 
	{
		Epic::InputData data
		{
			Epic::eInputDataType::Axis1D,
			m_DeviceName,
			Epic::StandardClock.Now(),
			{ std::move(axisData0) }
		};

		this->Input(data);
	}

	void OnInput(Epic::InputAxisData&& axisData0, Epic::InputAxisData&& axisData1) noexcept
	{
		Epic::InputData data
		{
			Epic::eInputDataType::Axis2D,
			m_DeviceName,
			Epic::StandardClock.Now(),
			{ std::move(axisData0), std::move(axisData1) }
		};

		this->Input(data);
	}

	void OnInput(Epic::InputAxisData&& axisData0, Epic::InputAxisData&& axisData1, Epic::InputAxisData&& axisData2) noexcept
	{
		Epic::InputData data
		{
			Epic::eInputDataType::Axis3D,
			m_DeviceName,
			Epic::StandardClock.Now(),
			{ std::move(axisData0), std::move(axisData1), std::move(axisData2) }
		};

		this->Input(data);
	}

public:
	using InputDelegate = Epic::Event<void(const Epic::InputData&)>;

public:
	InputDelegate Input;
};
