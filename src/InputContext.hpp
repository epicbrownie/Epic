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

#include <Epic/StringHash.hpp>
#include <Epic/STL/Vector.hpp>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class InputContext;
}

//////////////////////////////////////////////////////////////////////////////

// InputContext
class Epic::InputContext
{
public:
	using Type = Epic::InputContext;

private:
	using ActionList = Epic::STLVector<Epic::StringHash>;

private:
	Epic::StringHash m_ContextName;
	ActionList m_Actions;

public:
	explicit InputContext(const Epic::StringHash& contextName) noexcept
		: m_ContextName{ contextName }
	{
		assert(m_ContextName != Epic::Hash(""));
	}

public:
	inline const Epic::StringHash& GetContextName() const noexcept
	{
		return m_ContextName;
	}

public:
	inline auto size() const noexcept
	{
		return m_Actions.size();
	}

	inline auto begin() noexcept
	{
		return std::begin(m_Actions);
	}

	inline auto begin() const noexcept
	{
		return std::begin(m_Actions);
	}

	inline auto end() noexcept
	{
		return std::end(m_Actions);
	}

	inline auto end() const noexcept
	{
		return std::end(m_Actions);
	}

public:
	bool HasAction(const Epic::StringHash& actionName) const noexcept
	{
		std::find(begin(), end(), actionName) != end();
	}

	bool AddAction(const Epic::StringHash& actionName) noexcept
	{
		auto it = std::find(begin(), end(), actionName);

		if (it == end())
		{
			m_Actions.emplace_back(actionName);
			return true;
		}

		return false;
	}
	
	bool RemoveAction(const Epic::StringHash& actionName) noexcept
	{
		auto it = std::find(begin(), end(), actionName);

		if (it != end())
		{
			m_Actions.erase(it);
			return true;
		}

		return false;
	}

	void ClearActions() noexcept
	{
		m_Actions.clear();
	}
};
