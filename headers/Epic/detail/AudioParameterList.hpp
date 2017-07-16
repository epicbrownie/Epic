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
#include <Epic/AudioParameter.hpp>
#include <Epic/StringHash.hpp>
#include <Epic/STL/Map.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <memory>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class Sound;
	
	namespace detail
	{
		class AudioParameterList;
	}
}

//////////////////////////////////////////////////////////////////////////////

// AudioParameterList
class Epic::detail::AudioParameterList
{
public:
	using Type = Epic::detail::AudioParameterList;

	friend class Epic::Sound;

private:
	using ParamPtr = Epic::UniquePtr<Epic::AudioParameter>;
	using ParameterMap = Epic::STLUnorderedMap<Epic::StringHash, ParamPtr>;

private:
	ParameterMap m_Params;

private:
	static AudioParameter s_NullParameter;

protected:
	AudioParameterList() noexcept { };
	AudioParameterList(const Type&) = default;
	Type& operator = (const Type&) = default;

protected:
	void Initialize(FMOD::Studio::EventDescription* pDesc, FMOD::Studio::EventInstance* pInstance) noexcept
	{
		m_Params.clear();

		int paramCount = 0;
		pDesc->getParameterCount(&paramCount);

		for (int i = 0; i < paramCount; ++i)
		{
			FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
			if (FMODCHECK(pDesc->getParameterByIndex(i, &paramDesc)) &&
				paramDesc.type == FMOD_STUDIO_PARAMETER_GAME_CONTROLLED)
			{
				m_Params[Epic::Hash(paramDesc.name)] =
					Epic::MakeUnique<Epic::AudioParameter>(pInstance, paramDesc.index, paramDesc.minimum, paramDesc.maximum);
			}
		}
	}

public:
	const Epic::AudioParameter& operator [] (const Epic::StringHash id) const
	{
		auto it = m_Params.find(id);
		if (it == std::end(m_Params))
			return s_NullParameter;

		return *(*it).second;
	}

	Epic::AudioParameter& operator [] (const Epic::StringHash id)
	{
		auto it = m_Params.find(id);
		if (it == std::end(m_Params))
			return s_NullParameter;

		return *(*it).second;
	}
};
