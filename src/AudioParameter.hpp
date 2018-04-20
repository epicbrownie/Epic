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

#include <Epic/detail/FMODInclude.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class AudioParameter;
}

//////////////////////////////////////////////////////////////////////////////

// AudioParameter
class Epic::AudioParameter
{
public:
	using Type = Epic::AudioParameter;

private:
	FMOD::Studio::EventInstance* m_pInstance;
	int m_Index;
	float m_MinValue, m_MaxValue;

public:
	constexpr AudioParameter() noexcept
		: m_pInstance{ nullptr }, m_Index{ -1 }, 
		  m_MinValue{ 0.0f }, m_MaxValue{ 0.0f }
	{ }

	constexpr AudioParameter(FMOD::Studio::EventInstance* pInstance, int index, float minValue, float maxValue) noexcept
		: m_pInstance{ pInstance }, m_Index{ index }, 
		  m_MinValue{ minValue }, m_MaxValue{ maxValue }
	{ }

	~AudioParameter() noexcept = default;

	constexpr AudioParameter(const Type&) noexcept = default;
	constexpr AudioParameter(Type&&) noexcept = default;

public:
	Type& operator = (const Type&) noexcept = default;
	Type& operator = (Type&&) noexcept = default;

	inline Type& operator = (float value) noexcept
	{
		Set(value);
		return *this;
	}

	inline operator float() const noexcept
	{
		return Get();
	}

	inline explicit operator bool() const noexcept
	{
		return IsValid();
	}

public:
	inline bool IsValid() const noexcept
	{
		return m_pInstance != nullptr;
	}

	constexpr float MinValue() const
	{
		return m_MinValue;
	}

	constexpr float MaxValue() const
	{
		return m_MaxValue;
	}

	float Get() const noexcept
	{
		if (!m_pInstance)
			return 0.0f;

		float value;
		FMODCHECK(m_pInstance->getParameterValueByIndex(m_Index, &value));

		return value;
	}

	inline void Set(float value) noexcept
	{
		if (m_pInstance)
			FMODCHECK(m_pInstance->setParameterValueByIndex(m_Index, value));
	}
};
