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

	explicit DeviceResolver(const Epic::StringHash& device)
		: m_DeviceName{ device }
	{ }

	virtual ~DeviceResolver() noexcept = default;

public:
	inline const Epic::StringHash& GetDeviceName() const noexcept
	{
		return m_DeviceName;
	}

public:
	bool Resolve(const InputData& data) const noexcept
	{
		return data.Device == m_DeviceName;
	}
};
