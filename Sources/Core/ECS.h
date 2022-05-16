#pragma once
#include "PCH.h"
#include <Core/Utility.h>

namespace anya
{
	template <typename T>
	using is_component = std::is_class<T>;
	template <typename T>
	constexpr bool is_component_v = is_component<T>::value;

	template <typename T>
	using is_archetype = std::conjunction<utils::is_tuple<T>, utils::is_tuple_contains_only_class<T>>;
	template <typename T>
	constexpr bool is_archetype_v = is_archetype<T>::value;

	template <typename T>
	using is_one_of_comp_arche = std::disjunction<is_component<T>, is_archetype<T>>;
	template <typename T>
	constexpr bool is_one_of_comp_arche_v = is_one_of_comp_arche<T>::value;

	template <typename T>
	concept Component = is_one_of_comp_arche_v<T>;

	template <typename T>
	concept Archetype = is_archetype_v<T>;

	template<Archetype ArchetypeT, Component ComponentT>
	using is_archetype_contains = utils::is_tuple_element<ArchetypeT, ComponentT>;

	template<Archetype ArchetypeT, Component ComponentT>
	constexpr bool is_archetype_contains_v = is_archetype_contains<ArchetypeT, ComponentT>::value;

	template <typename ArchetypeT, typename ComponentT>
	concept ArchetypeContains = is_archetype_v<ArchetypeT> && is_archetype_contains_v<ArchetypeT, ComponentT>;

	enum class Entity : uint64 {};
	constexpr Entity INVALID_ENTITY_HANDLE = static_cast<Entity>(0);
	constexpr size_t DEFAULT_COMPONENT_POOL_SIZE = 16;
	constexpr bool USE_RANDOM_NUM_FOR_ENTITY_HANDLE = false;

	using ComponentID = size_t;
	constexpr ComponentID INVALID_COMPONET_ID = 0;

	inline Entity GenerateEntity()
	{
		using EntityUnderlyingType = std::underlying_type_t<Entity>;
		if (USE_RANDOM_NUM_FOR_ENTITY_HANDLE)
		{
			static thread_local std::mt19937_64 generator(
				std::hash<std::thread::id>{}(std::this_thread::get_id()));

			std::uniform_int_distribution<EntityUnderlyingType> dist(std::numeric_limits<EntityUnderlyingType>::min() + 1, std::numeric_limits<EntityUnderlyingType>::max());
			return static_cast<Entity>(dist(generator));
		}

		static std::atomic<EntityUnderlyingType> handle = 1;
		return static_cast<Entity>(handle++);
	}

	class ComponentIDGenerator
	{
	public:
		ComponentIDGenerator() = delete;

		template <Component T>
		inline static ComponentID Value()
		{
			return InternalValue<T>();
		}

		template <Component T>
		inline static ComponentID Value(const T&)
		{
			return InternalValue<T>();
		}

	private:
		template <Component T>
		inline static ComponentID InternalValue()
		{
			static_assert(std::same_as<std::decay_t<T>, T>);
			static ComponentID internalID = ++idCounter;
			return internalID;
		}

	private:
		inline static size_t idCounter{ INVALID_COMPONET_ID };

	};

	struct ComponentInfo
	{
		ComponentID ID = INVALID_COMPONET_ID;
		std::wstring Name;
		size_t Size;
		size_t Alignment;
		/** If component is actually archetype, It must be contains one or more subcomponents. */
		std::vector<ComponentID> Subcomponents;
	};

	template <Component... T>
	std::vector<ComponentID> AcquireSubComponentsFromArchetype(const std::tuple<T...>& archetype)
	{
		auto initList = { ComponentIDGenerator::Value<T>()... };
		std::vector<ComponentID> subcomponents{ initList };

		return subcomponents;
	}

	template <Component T>
	class ComponentInfoGenerator
	{
	public:
		ComponentInfoGenerator() = delete;

		static ComponentInfo Generate()
		{
			ComponentInfo result{
				.ID = ComponentIDGenerator::Value<T>(),
				.Name = utils::AnsiToWString(typeid(T).name()),
				.Size = sizeof(T),
				.Alignment = alignof(T) };

			if constexpr (utils::is_tuple_v<T>)
			{
				result.Subcomponents = AcquireSubComponentsFromArchetype(T());
			}

			return result;
		}
	};

	class ComponentPoolBase
	{
	public:
		ComponentPoolBase(const ComponentInfo& componentInfo) :
			componentInfo(componentInfo)
		{
		}

		virtual ~ComponentPoolBase()
		{
		}

		const ComponentInfo& AcquireComponentInfo() const
		{
			return componentInfo;
		}

		virtual bool Contains(Entity entity) const = 0;
		virtual bool Create(Entity entity) = 0;
		virtual void Remove(Entity entity) = 0;
		virtual size_t Size() const = 0;
		virtual void Clear() = 0;

		virtual void Attach(Entity parent, Entity child) { }
		virtual void Detach(Entity target) {}

	private:
		ComponentInfo componentInfo;

	};

	template <Component T>
	class ComponentPoolImpl : public ComponentPoolBase
	{
	public:
		using Ref = std::reference_wrapper<T>;
		using ConstRef = std::reference_wrapper<const T>;
		using VectorRef = std::reference_wrapper<std::vector<T>>;
		using ConstVectorRef = std::reference_wrapper<const std::vector<T>>;
		using OptionalRef = std::optional<Ref>;
		using OptionalConstRef = std::optional<ConstRef>;
		using OptionalVectorRef = std::optional<VectorRef>;
		using OptionalConstVectorRef = std::optional<ConstVectorRef>;

	public:
		explicit ComponentPoolImpl(size_t reservedSize) :
			ComponentPoolBase(ComponentInfoGenerator<T>::Generate())
		{
			components.reserve(reservedSize);
			entities.reserve(reservedSize);
			lut.reserve(reservedSize);
		}

		OptionalConstRef operator[](size_t idx) const
		{
			if (idx < components.size())
			{
				return std::make_optional(std::cref(components[idx]));
			}

			return std::nullopt;
		}

		OptionalRef operator[](size_t idx)
		{
			if (idx < components.size())
			{
				return std::make_optional(std::ref(components[idx]));
			}

			return std::nullopt;
		}

		virtual bool Contains(Entity entity) const override
		{
			return (lut.find(entity) != lut.end());
		}

		virtual bool Create(Entity entity) override
		{
			if (bool bIsDuplicatedEntity = Contains(entity); !bIsDuplicatedEntity)
			{
				[[likely]]
				lut[entity] = components.size();
				components.emplace_back();
				entities.push_back(entity);

				return true;
			}

			return false;
		}

		virtual void Remove(Entity entity) override
		{
			if (bool bIsValidEntityHandle = entity != INVALID_ENTITY_HANDLE; 
				bIsValidEntityHandle)
			{
				if (auto lutItr = lut.find(entity); lutItr != lut.end())
				{
					size_t targetComponentIdx = lutItr->second;
					
					if (bool bIsLatestComponent = targetComponentIdx == (components.size() - 1); 
						!bIsLatestComponent)
					{
						components[targetComponentIdx] = std::move(components.back());
						entities[targetComponentIdx] = entities.back();
						lut[entities[targetComponentIdx]] = targetComponentIdx;
					}

					components.pop_back();
					entities.pop_back();
					lut.erase(lutItr);
				}
			}
		}

		OptionalConstRef GetComponent(Entity entity) const
		{
			if (bool bIsValidEntityHandle = entity != INVALID_ENTITY_HANDLE; 
				bIsValidEntityHandle)
			{
				if (bool bHasRelatedComponent = Contains(entity); 
					bHasRelatedComponent)
				{
					size_t targetComponentIdx = lut[entity];
					return ConstComponentRetType(components[targetComponentIdx]);
				}
			}

			return std::nullopt;
		}

		OptionalRef GetComponent(Entity entity)
		{
			if (bool bIsValidEntityHandle = entity != INVALID_ENTITY_HANDLE; bIsValidEntityHandle)
			{
				
				if (bool bHasRelatedComponent = Contains(entity); bHasRelatedComponent)
				{
					size_t targetComponentIdx = lut[entity];
					return components[targetComponentIdx];
				}
			}

			return std::nullopt;
		}

		const std::vector<T>& GetComponents() const
		{
			return components;
		}

		std::vector<T>& GetComponents()
		{
			return components;
		}

		Entity GetEntity(size_t componentIdx) const
		{
			return (componentIdx < entities.size()) ?
				entities[componentIdx] :
				INVALID_ENTITY_HANDLE;
		}

		inline size_t Size() const
		{
			return components.size();
		}

		inline void Clear()
		{
			components.clear();
			entities.clear();
			lut.clear();
		}

		inline bool CheckRelationBetween(Entity entity, const T& component) const
		{
			if (!components.empty())
			{
				if (entity != INVALID_ENTITY_HANDLE)
				{
					if (auto lutItr = lut.find(entity); lutItr != lut.end())
					{
						return &components[lutItr->second] == &component;
					}
				}

			}

			return false;
		}

		inline bool CheckValidationOf(const T& component) const
		{
			auto foundItr = std::find_if(components.begin(), components.end(), [&](const T& validComponent)
				{
					return (&validComponent == &component);
				});

			return foundItr != components.end();
		}

		inline void UpdateLUT()
		{
			for (size_t idx = 0; idx < entities.size(); ++idx)
			{
				lut[entities[idx]] = idx;
			}
		}

	protected:
		/** Move Components/Entities elements block [blockBegin, blockEnd) to destination. */
		inline void MoveElementBlock(size_t blockBegin, size_t blockEnd, size_t destination)
		{
			bool bIsValidRange = 
				(blockBegin >= 0 && blockBegin < Size()) && 
				(blockEnd >= 0 && blockEnd <= Size());

			if (bIsValidRange)
			{
				if (blockBegin > blockEnd)
				{
					std::swap(blockBegin, blockEnd);
				}

				auto compBeginItr = components.begin();
				std::move(
					compBeginItr + blockBegin,
					compBeginItr + blockEnd,
					compBeginItr + destination);

				auto entitiyBeginItr = entities.begin();
				std::move(
					entitiyBeginItr + blockBegin,
					entitiyBeginItr + blockEnd,
					entitiyBeginItr + destination);
			}
		}

		inline void MoveElementBlockFrom(std::vector<T> moveComponents, std::vector<Entity> moveEntities, size_t moveAt)
		{
			bool bIsValidPoint = moveAt >= 0 && moveAt < Size();

			if (bIsValidPoint)
			{
				if (moveAt + moveComponents.size() - 1 >= Size())
				{
					size_t newSize = moveAt + moveComponents.size();
					components.resize(newSize);
					entities.resize(newSize);
				}

				std::move(
					moveComponents.begin(),
					moveComponents.end(),
					components.begin() + moveAt);

				std::move(
					moveEntities.begin(),
					moveEntities.end(),
					entities.begin() + moveAt);
			}
		}

	protected:
		std::vector<T> components;
		/* Entities[idx] == Entities[LUT[Entities[idx]]] **/
		std::vector<Entity> entities; 
		std::unordered_map<Entity, size_t> lut;

	};

	template <Component T>
	class ComponentPool : public ComponentPoolImpl<T>
	{
	public:
		explicit ComponentPool(size_t reservedSize = (DEFAULT_COMPONENT_POOL_SIZE)) :
			ComponentPoolImpl<T>(reservedSize)
		{
		}
	};

	class ComponentPoolProxy
	{
	public:
		~ComponentPoolProxy()
		{
			for (auto& registered : registry)
			{
				delete registered.second;
			}
		}

		ComponentPoolProxy(const ComponentPoolProxy&) = delete;
		ComponentPoolProxy& operator=(const ComponentPoolProxy&) = delete;

		ComponentPoolProxy(ComponentPoolProxy&& rhs) noexcept :
			registry(std::move(rhs.registry))
		{
		}

		static ComponentPoolProxy& Get()
		{
			static ComponentPoolProxy* instance = nullptr;
			if (instance == nullptr)
			{
				instance = new ComponentPoolProxy();
			}

			return *instance;
		}

		template <Component T>
		void Register(size_t reservedSize = (DEFAULT_COMPONENT_POOL_SIZE))
		{
			const ComponentID componentID = ComponentIDGenerator::Value<T>();
			if (registry.find(componentID) == registry.end())
			{
				registry[componentID] = new ComponentPool<std::decay_t<T>>(reservedSize);
			}
		}

		/** Return ComponentID it self when Component is independent to other archetype. */
		/** Otherwise, It will be return archetype's ComponentID which is have a queried component. */
		/** Else if, Entity does not have queried component, it will be return INVALID_COMPONENT_ID */
		ComponentID HasComponent(const Entity entity, const ComponentID componentID) const
		{
			if (componentID != INVALID_COMPONET_ID)
			{
				if (const auto entityLUTItr = entityLUT.find(entity);
					entityLUTItr != entityLUT.cend())
				{
					const auto& componentLUT = entityLUTItr->second;
					if (const auto componentLUTItr = componentLUT.find(componentID);
						componentLUTItr != componentLUT.cend())
					{
						return componentLUTItr->second;
					}
				}
			}

			return INVALID_COMPONET_ID;
		}

		/** Return ComponentID it self when Component is independent to other archetype. */
		/** Otherwise, It will be return archetype's ComponentID which is have a queried component. */
		/** Else if, Entity does not have queried component, it will be return INVALID_COMPONENT_ID */
		template <Component T>
		inline ComponentID HasComponent(const Entity entity) const
		{
			const ComponentID componentID = ComponentIDGenerator::Value<T>();
			return HasComponent(componentID, entity);
		}

		bool HasAllComponents(const Entity entity, const std::vector<ComponentID>& components) const
		{
			for (auto componentID : components)
			{
				auto actualComponent = HasComponent(entity, componentID);
				if (actualComponent != componentID)
				{
					return false;
				}
			}

			return true;
		}

		template <Component... T>
		bool HasAllComponents(const Entity entity)
		{
			auto initList = { ComponentIDGenerator::Value<T>()... };
			std::vector<ComponentID> components = initList;
			return HasAllComponents(entity, components);
		}

		bool HasAnyComponents(const Entity entity, const std::vector<ComponentID>& components) const
		{
			for (auto componentID : components)
			{
				auto actualComponent = HasComponent(entity, componentID);
				if (actualComponent == componentID)
				{
					return true;
				}
			}

			return false;
		}

		template <Component... T>
		bool HasAnyComponents(const Entity entity)
		{
			auto initList = { ComponentIDGenerator::Value<T>()... };
			std::vector<ComponentID> components = initList;
			return HasAnyComponents(entity, components);
		}

		bool CreateComponent(const Entity entity, const ComponentID componentID)
		{
			if (componentID != INVALID_COMPONET_ID)
			{
				if (HasComponent(entity, componentID) == INVALID_COMPONET_ID)
				{
					auto foundPool = Acquire(componentID);
					if (foundPool != nullptr)
					{
						const ComponentInfo& componentInfo = foundPool->AcquireComponentInfo();
						if (!HasAnyComponents(entity, componentInfo.Subcomponents))
						{
							if (foundPool->Create(entity))
							{
								const auto foundEntityLUTItr = entityLUT.find(entity);
								if (foundEntityLUTItr == entityLUT.cend())
								{
									entityLUT[entity] = {};
								}

								auto& componentLUT = entityLUT[entity];
								componentLUT[componentID] = componentID;
								for (const auto subcomponentID : componentInfo.Subcomponents)
								{
									componentLUT[subcomponentID] = componentID;
								}

								return true;
							}
						}
					}
				}
			}

			return false;
		}

		template <Component T>
		inline bool CreateComponent(const Entity entity)
		{
			const ComponentID componentID = ComponentIDGenerator::Value<T>();
			return CreateComponent(entity, componentID);
		}

		/** It will been removed only when target component is equal with actual component. */
		void Remove(Entity target, const ComponentID componentID)
		{
			if (componentID != INVALID_COMPONET_ID)
			{
				const ComponentID actualComponentID = HasComponent(target, componentID);
				if (actualComponentID == componentID)
				{
					auto acquiredPool = Acquire(actualComponentID);
					if (acquiredPool != nullptr)
					{
						acquiredPool->Remove(target);

						auto& componentLUT = entityLUT[target];

						// Make subcomponents to expired.
						const auto& info = acquiredPool->AcquireComponentInfo();
						for (const auto componentID : info.Subcomponents)
						{
							componentLUT.erase(componentID);
						}

						// Make expired it self
						componentLUT.erase(componentID);
					}
				}
			}
		}

		template <Component T>
		inline void Remove(Entity target)
		{
			const ComponentID componentID = ComponentIDGenerator::Value<T>();
			return Remove(target, componentID);
		}

		void Attach(const ComponentID componentID, Entity parent, Entity child)
		{
			if (componentID != INVALID_COMPONET_ID)
			{
				const ComponentID parentsActualComponentID = HasComponent(parent, componentID);
				const ComponentID childsActualComponentID = HasComponent(child, componentID);

				if (parentsActualComponentID != INVALID_COMPONET_ID && childsActualComponentID != INVALID_COMPONET_ID)
				{
					if (parentsActualComponentID == childsActualComponentID)
					{
						const ComponentID actualComponentID = parentsActualComponentID;
						auto acquiredPool = Acquire(actualComponentID);
						if (acquiredPool != nullptr)
						{
							acquiredPool->Attach(parent, child);
						}
					}
				}
			}
		}

		template <Component T>
		inline void Attach(Entity parent, Entity child)
		{
			const ComponentID componentID = ComponentIDGenerator::Value<T>();
			return Attach(componentID, parent, child);
		}

		void Detach(Entity target, const ComponentID componentID)
		{
			if (componentID != INVALID_COMPONET_ID)
			{
				const ComponentID actualComponentID = HasComponent(target, componentID);
				if (actualComponentID != INVALID_COMPONET_ID)
				{
					auto acquiredPool = Acquire(actualComponentID);
					if (acquiredPool != nullptr)
					{
						acquiredPool->Detach(target);
					}
				}
			}
		}

		template <Component T>
		inline void Detach(Entity target)
		{
			const ComponentID componentID = ComponentIDGenerator::Value<T>();
			return Detach(target, componentID);
		}

		template <Component T>
		ComponentPool<T>::OptionalRef QueryComponent(Entity entity)
		{
			ComponentID targetComponentID = ComponentIDGenerator::Value<T>();
			ComponentID actualComponentID = HasComponent<T>(entity);
			if (targetComponentID == actualComponentID)
			{
				ComponentPool<T>* componentPool = static_cast<ComponentPool<T>*>(Acquire(targetComponentID));
				if (componentPool != nullptr)
				{
					return componentPool->GetComponent(entity);
				}
			}

			return std::nullopt;
		}

		template <Component T>
		ComponentPool<T>::OptionalConstRef QueryComponent(Entity entity) const
		{
			ComponentID targetComponentID = ComponentIDGenerator::Value<T>();
			ComponentID actualComponentID = HasComponent<T>(entity);
			if (targetComponentID == actualComponentID)
			{
				ComponentPool<T>* componentPool = static_cast<ComponentPool<T>*>(Acquire(targetComponentID));
				if (componentPool != nullptr)
				{
					return componentPool->GetComponent(entity);
				}
			}

			return std::nullopt;
		}

		template <Component T>
		std::vector<T> QueryComponents()
		{
			ComponentPool<T>& componentPool = Acquire<T>();
			return componentPool->GetComponents();
		}

		template <Component T>
		const std::vector<T>& QueryComponents() const
		{
			const ComponentPool<T>& componentPool = Acquire<T>();
			return componentPool->GetComponents();
		}

	private:
		ComponentPoolProxy() = default;

		ComponentPoolBase* Acquire(const ComponentID componentID) const
		{
			auto itr = registry.find(componentID);
			if (itr == registry.end())
			{
				assert(false && "Component does not registered.");
			}

			return itr->second;
		}

		template <Component T>
		ComponentPool<T>& Acquire()
		{
			const ComponentID componentID = ComponentIDGenerator::Value<T>();
			ComponentPool<T>* acquired = static_cast<ComponentPool<T>*>(Acquire(componentID));

			return (*acquired);
		}

		template <Component T>
		const ComponentPool<T>& Acquire() const
		{
			const ComponentID componentID = ComponentIDGenerator::Value<T>();
			const ComponentPool<T>* acquired = static_cast<ComponentPool<T>*>(Acquire(componentID));

			return (*acquired);
		}

	private:
		std::unordered_map<ComponentID, ComponentPoolBase*> registry;
		// Entity -> Own Component -> Actual Component(or Archetype)
		std::unordered_map<Entity, std::unordered_map<ComponentID, ComponentID>> entityLUT;

	};

	static std::vector<Entity> FilterAnyOf(const std::vector<Entity>& entities, const ComponentPoolProxy& proxy, const std::vector<ComponentID>& componentIDs)
	{
		std::vector<Entity> filtered;
		filtered.reserve(entities.size());

		for (const Entity entity : entities)
		{
			if (proxy.HasAnyComponents(entity, componentIDs))
			{
				filtered.emplace_back(entity);
			}
		}

		return filtered;
	}

	template <Component... T>
	std::vector<Entity> FilterAnyOf(const std::vector<Entity>& entities, const ComponentPoolProxy& proxy)
	{
		auto initList = { ComponentIDGenerator::Value<T>()... };
		std::vector<ComponentID> componentsList = initList;
		return FilterAnyOf(entities, proxy, componentsList);
	}

	static std::vector<Entity> FilterAllOf(const std::vector<Entity>& entities, const ComponentPoolProxy& proxy, const std::vector<ComponentID>& componentIDs)
	{
		std::vector<Entity> filtered;
		filtered.reserve(entities.size());

		for (const Entity entity : entities)
		{
			if (proxy.HasAllComponents(entity, componentIDs))
			{
				filtered.emplace_back(entity);
			}
		}

		return filtered;
	}

	template <Component... T>
	std::vector<Entity> FilterAllOf(const std::vector<Entity>& entities, const ComponentPoolProxy& proxy)
	{
		auto initList = { ComponentIDGenerator::Value<T>()... };
		std::vector<ComponentID> componentsList = initList;
		return FilterAllOf(entities, proxy, componentsList);
	}

	template <typename QueryType, typename Archetype>
	QueryType& QueryArchetypeData(Archetype& object)
	{
		return std::get<QueryType>(object);
	}
}

#define DeclareComponent(ComponentType) \
	struct ComponentType##Registeration \
	{ \
		ComponentType##Registeration() \
		{ \
			auto& proxy = anya::ComponentPoolProxy::Get(); \
			proxy.Register<ComponentType>(); \
		}	\
	private: \
		static ComponentType##Registeration registeration; \
	};\

#define RegisterComponent(ComponentType) ComponentType##Registeration ComponentType##Registeration::registeration;

#define DeclareArchetype(ArchetypeName, ...) \
using ArchetypeName = std::tuple<__VA_ARGS__>; \
DeclareComponent(ArchetypeName);

#define RegisterArchetype(ArchetypeName) RegisterComponent(ArchetypeName)