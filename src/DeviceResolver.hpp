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
	class DeviceResolver;
}

//////////////////////////////////////////////////////////////////////////////

// DeviceResolver
class Epic::DeviceResolver : public Epic::InputResolver
{
public:
	using Type = Epic::DeviceResolver;
	using Base = Epic::InputResolver;

private:
	Epic::StringHash m_DeviceName;

public:
	DeviceResolver() noexcept = delete;
	DeviceResolver(Type&) noexcept = delete;
	DeviceResolver(Type&&) noexcept = delete;

	Type& operator = (const Type&) = delete;
	Type& operator = (Type&&) = delete;

	explicit DeviceResolver(Epic::StringHash device) noexcept
		: m_DeviceName{ device }
	{ }

	~DeviceResolver() noexcept = default;

public:
	inline Epic::StringHash GetDeviceName() const noexcept
	{
		return m_DeviceName;
	}

public:
	bool Resolve(const InputData& data) const noexcept override
	{
		return data.Device == m_DeviceName;
	}
};
