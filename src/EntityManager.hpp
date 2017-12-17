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

#include <Epic/detail/EntityComponentIterator.hpp>
#include <Epic/detail/EntityComponentView.hpp>
#include <Epic/detail/EntityManagerFwd.hpp>
#include <Epic/Entity.hpp>
#include <Epic/EntitySystem.hpp>
#include <Epic/Event.hpp>
#include <Epic/StringHash.hpp>
#include <Epic/STL/Vector.hpp>
#include <Epic/STL/Map.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <algorithm>
#include <functional>

//////////////////////////////////////////////////////////////////////////////

// EntityManager
class Epic::EntityManager
{
public:
	using Type = Epic::EntityManager;

public:
	constexpr static Epic::StringHash NoEntityName = Entity::NoEntityName;

private:
	using EntityPtr = Epic::UniquePtr<Entity>;
	using EntityList = Epic::STLVector<EntityPtr>;
	using EntityNameMap = Epic::STLUnorderedMap<Epic::StringHash, EntityPtr::pointer>;
	using EntityIDMap = Epic::STLUnorderedMap<EntityID, EntityPtr::pointer>;
	using SystemPtr = Epic::UniquePtr<EntitySystem>;
	using SystemList = Epic::STLVector<SystemPtr>;

private:
	EntityID m_NextID;
	EntityList m_Entities;
	EntityNameMap m_NameEntityMap;
	EntityIDMap m_IDEntityMap;
	SystemList m_Systems;

public:
	EntityManager() noexcept 
		: m_NextID{ 1 } 
	{ }

	~EntityManager() noexcept
	{
		DestroySystems();
		DestroyEntities();
	}

private:
	inline void OnEntityCreated(EntityPtr::pointer pEntity) noexcept
	{
		this->EntityCreated(pEntity);
	}

	inline void OnEntityDestroyed(EntityPtr::pointer pEntity) noexcept
	{
		this->EntityDestroyed(pEntity);
	}

	void _DestroyEntity(EntityPtr::pointer pEntity) noexcept
	{
		m_NameEntityMap.erase(pEntity->GetName());
		m_IDEntityMap.erase(pEntity->GetID());

		auto it = std::find_if(
			std::begin(m_Entities),
			std::end(m_Entities),
			[&](const auto& p) { return p.get() == pEntity; });

		if (it != std::end(m_Entities))
			m_Entities.erase(it);
	}

	inline auto _GetSystemByPtr(SystemPtr::pointer p) noexcept
	{
		return std::find_if(std::begin(m_Systems), std::end(m_Systems),
							[&](const SystemPtr& pSystem) { return pSystem.get() == p; });
	}

	inline void _ConstructSystem(SystemPtr::pointer pSystem) noexcept
	{
		EntityCreated.Connect(pSystem, &EntitySystem::OnEntityCreated);
		EntityDestroyed.Connect(pSystem, &EntitySystem::OnEntityDestroyed);

		for (auto pEntity : All())
		{
			pEntity->ComponentAttached.Connect(pSystem, &EntitySystem::EntityComponentAttached);
			pEntity->ComponentDetached.Connect(pSystem, &EntitySystem::EntityComponentDetached);
		}

		pSystem->InitialUpdate();
	}

	inline void _DestroySystem(SystemPtr::pointer pSystem) noexcept
	{
		EntityCreated.DisconnectAll(pSystem);
		EntityDestroyed.DisconnectAll(pSystem);

		for (auto pEntity : All())
		{
			pEntity->ComponentAttached.DisconnectAll(pSystem);
			pEntity->ComponentDetached.DisconnectAll(pSystem);
		}
	}

public:
	inline size_t GetEntityCount() const noexcept
	{
		return m_Entities.size();
	}

	inline size_t GetSystemCount() const noexcept
	{
		return m_Systems.size();
	}

	inline EntityPtr::pointer GetEntity(const EntityID id) noexcept
	{
		auto it = m_IDEntityMap.find(id);

		return (it != std::end(m_IDEntityMap)) ? it->second : nullptr;
	}

	inline const EntityPtr::pointer GetEntity(const EntityID id) const noexcept
	{
		auto it = m_IDEntityMap.find(id);

		return (it != std::end(m_IDEntityMap)) ? it->second : nullptr;
	}

	inline EntityPtr::pointer GetEntity(Epic::StringHash name) noexcept
	{
		auto it = m_NameEntityMap.find(name);

		return (it != std::end(m_NameEntityMap)) ? it->second : nullptr;
	}

	inline const EntityPtr::pointer GetEntity(Epic::StringHash name) const noexcept
	{
		auto it = m_NameEntityMap.find(name);

		return (it != std::end(m_NameEntityMap)) ? it->second : nullptr;
	}

	inline EntityPtr::pointer GetEntityByIndex(size_t index) noexcept
	{
		if (index >= 0 && index < m_Entities.size())
			return m_Entities[index].get();

		return nullptr;
	}

	inline const EntityPtr::pointer GetEntityByIndex(size_t index) const noexcept
	{
		if (index >= 0 && index < m_Entities.size())
			return m_Entities[index].get();

		return nullptr;
	}

public:
	inline SystemPtr::pointer GetSystemByIndex(size_t index) noexcept
	{
		if (index >= 0 && index < m_Systems.size())
			return m_Systems[index].get();

		return nullptr;
	}

	inline const SystemPtr::pointer GetSystemByIndex(size_t index) const noexcept
	{
		if (index >= 0 && index < m_Systems.size())
			return m_Systems[index].get();

		return nullptr;
	}

	template<class System>
	inline System* GetSystemByIndexAs(size_t index) noexcept
	{
		return static_cast<System*>(GetSystemByIndex(index));
	}

	template<class System>
	inline const System* GetSystemByIndexAs(size_t index) const noexcept
	{
		return static_cast<System*>(GetSystemByIndex(index));
	}

public:
	EntityPtr::pointer CreateEntity(Epic::StringHash name = NoEntityName) noexcept
	{
		m_Entities.emplace_back(Epic::MakeUnique<Entity>(this, name, m_NextID++));
		EntityPtr::pointer pEntity = m_Entities.back().get();

		if (name != NoEntityName)
			m_NameEntityMap[name] = pEntity;

		m_IDEntityMap[pEntity->GetID()] = pEntity;

		OnEntityCreated(pEntity);

		return pEntity;
	}

	void DestroyEntity(EntityPtr::pointer pEntity, bool destroyNow = false) noexcept
	{
		if (!pEntity)
			return;

		if (pEntity->IsDestroyPending())
		{
			if (destroyNow)
				_DestroyEntity(pEntity);
		}
		else
		{
			pEntity->Destroy();
			OnEntityDestroyed(pEntity);

			if (destroyNow)
				_DestroyEntity(pEntity);
		}
	}

	inline void DestroyEntity(Epic::StringHash name, bool destroyNow = false) noexcept
	{
		DestroyEntity(GetEntity(name), destroyNow);
	}

	inline void DestroyEntity(EntityID id, bool destroyNow = false) noexcept
	{
		DestroyEntity(GetEntity(id), destroyNow);
	}

	void DestroyEntities() noexcept
	{
		m_NameEntityMap.clear();
		m_IDEntityMap.clear();

		for (auto& pEntity : m_Entities)
		{
			pEntity->Destroy();
			OnEntityDestroyed(pEntity.get());
		}

		m_Entities.clear();
		m_NextID = 1;
	}

public:
	template<class System, class... Args>
	System* CreateSystem(Args&&... args) noexcept
	{
		m_Systems.emplace_back(Epic::MakeImpl<EntitySystem, System>(this, std::forward<Args>(args)...));

		auto pSystem = m_Systems.back().get();

		_ConstructSystem(pSystem);
		
		return static_cast<System*>(pSystem);
	}

	void DestroySystem(SystemPtr::pointer pSystem) noexcept
	{
		auto it = _GetSystemByPtr(pSystem);
		if (it != std::end(m_Systems))
		{
			_DestroySystem(it->get());
			m_Systems.erase(it);
		}
	}

	void DestroySystems() noexcept
	{
		for (auto& pSystem : m_Systems)
			_DestroySystem(pSystem.get());

		m_Systems.clear();
	}

public:
	template<class... Components>
	Epic::detail::EntityComponentView<Components...> Each(bool includeDestroyed = false) noexcept
	{
		return Epic::detail::EntityComponentView<Components...>
		{
			{ this, 0, includeDestroyed },
			{ this, GetEntityCount(), includeDestroyed }
		};
	}

	template<class... Components>
	Epic::detail::ConstEntityComponentView<Components...> Each(bool includeDestroyed = false) const noexcept
	{
		return Epic::detail::ConstEntityComponentView<Components...>
		{
			{ this, 0, includeDestroyed },
			{ this, GetEntityCount(), includeDestroyed }
		};
	}

	template<class... Components>
	void Each(std::function<void(Entity&, Components&...)> fn, bool includeDestroyed = false)
	{
		if (includeDestroyed)
		{
			for (auto& pEntity : m_Entities)
			{
				auto& entity = *pEntity;
				if (entity.Has<Components...>())
					fn(entity, entity.Get<Components>()...);
			}
		}
		else
		{
			for (auto& pEntity : m_Entities)
			{
				auto& entity = *pEntity;
				if (entity.Has<Components...>() && !entity.IsDestroyPending())
					fn(entity, entity.Get<Components>()...);
			}
		}
	}

	template<class... Components>
	void Each(std::function<void(const Entity&, const Components&...)> fn, bool includeDestroyed = false) const
	{
		if (includeDestroyed)
		{
			for (auto& pEntity : m_Entities)
			{
				auto& entity = *pEntity;
				if (entity.Has<Components...>())
					fn(entity, entity.Get<Components>()...);
			}
		}
		else
		{
			for (auto& pEntity : m_Entities)
			{
				auto& entity = *pEntity;
				if (entity.Has<Components...>() && !entity.IsDestroyPending())
					fn(entity, entity.Get<Components>()...);
			}
		}
	}

	Epic::detail::EntityComponentView<> All(bool includeDestroyed = false) noexcept
	{
		return Epic::detail::EntityComponentView<>
		{
			{ this, 0, includeDestroyed },
			{ this, GetEntityCount(), includeDestroyed }
		};
	}

	Epic::detail::ConstEntityComponentView<> All(bool includeDestroyed = false) const noexcept
	{
		return Epic::detail::ConstEntityComponentView<>
		{
			{ this, 0, includeDestroyed },
			{ this, GetEntityCount(), includeDestroyed }
		};
	}

	void All(std::function<void(Entity&)> fn, bool includeDestroyed = false)
	{
		if (includeDestroyed)
		{
			for (auto& pEntity : m_Entities)
				fn(*pEntity);
		}
		else
		{
			for (auto& pEntity : m_Entities)
			{
				if (!pEntity->IsDestroyPending())
					fn(*pEntity);
			}
		}
	}

	void All(std::function<void(const Entity&)> fn, bool includeDestroyed = false) const
	{
		if (includeDestroyed)
		{
			for (auto& pEntity : m_Entities)
				fn(*pEntity);
		}
		else
		{
			for (auto& pEntity : m_Entities)
			{
				if (!pEntity->IsDestroyPending())
					fn(*pEntity);
			}
		}
	}

public:
	void Update()
	{
		// Update Systems
		for (auto& pSystem : m_Systems)
			pSystem->Update();

		// Update Entity list
		m_Entities.erase(std::remove_if(
			std::begin(m_Entities),
			std::end(m_Entities),
			[&] (auto& pEntity)
		{
			if (pEntity->IsDestroyPending())
			{
				m_NameEntityMap.erase(pEntity->GetName());
				m_IDEntityMap.erase(pEntity->GetID());
				return true;
			}

			return false;
		}), std::end(m_Entities));
	}

private:
	using EntityChangedDelegate = Epic::Event<void(EntityPtr::pointer)>;

public:
	EntityChangedDelegate EntityCreated;
	EntityChangedDelegate EntityDestroyed;
};
