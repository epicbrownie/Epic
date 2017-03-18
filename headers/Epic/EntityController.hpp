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
#include <Epic/Event.hpp>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class EntityController;
}

//////////////////////////////////////////////////////////////////////////////

// EntityController
class Epic::EntityController
{
public:
	using Type = Epic::EntityController;

private:
	friend class Epic::EntityControllerSystem;

private:
	// This member will be set by EntityControllerSystem owner
	Epic::EntityManager* m_pEntityManager;
	
public:
	EntityController() noexcept { }
	virtual ~EntityController() { }

public:
	constexpr Epic::EntityManager* GetEntityManager() const noexcept
	{
		return m_pEntityManager;
	}

public:
	virtual void Update() = 0;

protected:
	virtual void OnEntityCreated(Epic::Entity*) { }
	virtual void OnEntityDestroyed(Epic::Entity*) { }
};
