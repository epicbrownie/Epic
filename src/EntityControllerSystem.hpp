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
//
//    This simple ECS system was inspired by Sam Bloomberg's ECS system
//        available for download at: https://github.com/redxdev/ECS
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Epic/detail/EntityControllerSystemFwd.hpp>
#include <Epic/EntityManager.hpp>
#include <Epic/EntityController.hpp>
#include <Epic/Event.hpp>
#include <Epic/STL/Vector.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <algorithm>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

// EntityControllerSystem
class Epic::EntityControllerSystem
{
public:
	using Type = Epic::EntityControllerSystem;

private:
	using ControllerPtr = Epic::ImplPtr<Epic::EntityController>;
	using ControllerList = Epic::STLVector<ControllerPtr>;
	
private:
	Epic::EntityManager* m_pEntityManager;
	ControllerList m_Controllers;

public:
	explicit EntityControllerSystem(Epic::EntityManager* pEntityManager) noexcept
		: m_pEntityManager{ pEntityManager }
	{
		assert(m_pEntityManager);
	}

	~EntityControllerSystem() noexcept
	{
		DestroyAllControllers();
	}

private:
	inline auto _GetControllerByPtr(ControllerPtr::pointer p) noexcept
	{
		return std::find_if(std::begin(m_Controllers), std::end(m_Controllers),
			[&](const ControllerPtr& pController) { return pController.get() == p; });
	}

public:
	inline void Update()
	{
		for (auto& pController : m_Controllers)
			pController->Update();
	}

public:
	inline size_t GetControllerCount() const noexcept
	{
		return m_Controllers.size();
	}

	ControllerPtr::pointer GetControllerByIndex(const size_t index) noexcept
	{
		if (index >= 0 && index < m_Controllers.size())
			return m_Controllers[index].get();

		return nullptr;
	}

	template<class Controller>
	inline Controller* GetControllerByIndex(const size_t index) noexcept
	{
		return static_cast<Controller*>(GetControllerByIndex(index));
	}

	template<class Controller, class... Args>
	Controller* CreateController(Args&&... args) noexcept
	{
		m_Controllers.emplace_back(
			Epic::MakeImpl<Epic::EntityController, Controller>(
				m_pEntityManager, std::forward<Args>(args)...));
		
		return static_cast<Controller*>(m_Controllers.back().get());
	}

	void DestroyController(ControllerPtr::pointer pController) noexcept
	{
		auto it = _GetControllerByPtr(pController);
		if (it != std::end(m_Controllers))
			m_Controllers.erase(it);
	}

	void DestroyAllControllers() noexcept
	{
		m_Controllers.clear();
	}
};