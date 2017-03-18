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
#include <Epic/InputDevice.hpp>
#include <Epic/StringHash.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <Epic/STL/Vector.hpp>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class InputDeviceManager;
}

//////////////////////////////////////////////////////////////////////////////

// InputDeviceManager
class Epic::InputDeviceManager
{
public:
	using Type = Epic::InputDeviceManager;

private:
	using DevicePtr = Epic::UniquePtr<Epic::InputDevice>;
	using DeviceList = Epic::STLVector<DevicePtr>;
	
private:
	DeviceList m_Devices;
	
public:
	InputDeviceManager() noexcept = default;

public:
	InputDeviceManager(const Type&) = delete;
	Type& operator = (const Type&) = delete;
	
public:
	template<class DeviceType, class... Args>
	DeviceType* CreateDevice(const Epic::StringHash& deviceName, Args&&... args)
	{
		auto pDevice = Epic::MakeImpl<Epic::InputDevice, DeviceType>(deviceName, std::forward<Args>(args)...);
		auto pDevicePtr = static_cast<DeviceType*>(pDevice.get());

		m_Devices.emplace_back(std::move(pDevice));
		pDevicePtr->Input.Connect(this, &InputDeviceManager::OnDeviceInput);

		return pDevicePtr;
	}
	
	void DestroyDevice(const Epic::StringHash& deviceName) noexcept
	{
		for(auto it = std::begin(m_Devices); it != std::end(m_Devices); ++it)
		{
			if ((*it)->GetDeviceName() == deviceName)
			{
				m_Devices.erase(it);
				break;
			}
		}
	}

public:
	DevicePtr::pointer GetDeviceByName(const Epic::StringHash& deviceName) const noexcept
	{
		for (auto& pDevice : m_Devices)
		{
			if (pDevice->GetDeviceName() == deviceName)
				return pDevice.get();
		}

		return nullptr;
	}

	DevicePtr::pointer GetDeviceByAttribute(const Epic::StringHash& attribute, uint64_t value) const noexcept
	{
		for (auto& pDevice : m_Devices)
		{
			if (pDevice->GetDeviceAttribute(attribute) == value)
				return pDevice.get();
		}

		return nullptr;
	}

public:
	inline void UpdateDevices() noexcept
	{
		for (auto& pDevice : m_Devices)
			pDevice->Update();
	}

private:
	void OnDeviceInput(const Epic::InputData& data)
	{
		this->Input(data);
	}

public:
	using InputDelegate = Epic::Event<void(const Epic::InputData&)>;

public:
	InputDelegate Input;
};
