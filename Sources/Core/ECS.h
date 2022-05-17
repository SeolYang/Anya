#pragma once
#include "PCH.h"
#include <Core/Utility.h>

namespace sy
{
	template <typename T>
	using is_component = std::is_class<T>;
	template <typename T>
	constexpr bool is_component_v = is_component<T>::value;

	template <typename T>
	using is_archetype = std::conjunction<utils::is_tuple<T>, utils::is_tuple_contains_only_class<T>, std::negation<utils::is_tuple_contains_other_tuple<T>>>;
	template <typename T>
	constexpr bool is_archetype_v = is_archetype<T>::value;

	template <typename T>
	concept Component = is_component_v<T>;

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

	using ComponentID = uint32;
	constexpr ComponentID INVALID_COMPONET_ID = 0;

	template <Component T>
	constexpr ComponentID QueryComponentID()
	{
		return INVALID_COMPONET_ID;
	}

	template <Component T>
	constexpr ComponentID QueryComponentID(const T&)
	{
		return QueryComponentID<T>;
	}

	struct ComponentInfo
	{
		ComponentID ID = INVALID_COMPONET_ID;
		std::wstring Name;
		size_t Size;
		size_t Alignment;
		/** If component is actually archetype, It must be contains one or more subcomponents. */
		std::vector<ComponentID> Subcomponents;

		template <Component T>
		static ComponentInfo Generate()
		{
			ComponentInfo result{
				.ID = QueryComponentID<T>(),
				.Name = utils::AnsiToWString(typeid(T).name()),
				.Size = sizeof(T),
				.Alignment = alignof(T) };

			if constexpr (is_archetype_v<T>)
			{
				auto subcomponentsArray = AcquireSubComponentsFromArchetype(T());
				result.Subcomponents = std::vector<ComponentID>(subcomponentsArray.cbegin(), subcomponentsArray.cend());
			}

			return result;
		}
	};

	template <Component... T>
	constexpr std::array<ComponentID, sizeof...(T)> AcquireSubComponentsFromArchetype(const std::tuple<T...>&)
	{
		std::array<ComponentID, sizeof...(T)> subcomponents{ QueryComponentID<T>()... };
		return subcomponents;
	}

	class ComponentPoolBase
	{
	public:
		ComponentPoolBase(const ComponentInfo& componentInfo) :
			componentInfo(componentInfo)
		{
		}

		virtual ~ComponentPoolBase() = default;

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
			ComponentPoolBase(ComponentInfo::Generate<T>())
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
					size_t targetComponentIdx = lut.find(entity)->second;
					return OptionalConstRef(std::cref(components[targetComponentIdx]));
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
					return std::ref(components[targetComponentIdx]);
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

	template <Component T>
	class ArchetypeArchive
	{
	public:
		using Func = std::function<std::optional<std::reference_wrapper<T>>(ComponentPoolBase*, Entity entity)>;

		ArchetypeArchive(const ArchetypeArchive&) = delete;
		ArchetypeArchive(ArchetypeArchive&&) = delete;
		ArchetypeArchive& operator=(const ArchetypeArchive&) = delete;
		ArchetypeArchive& operator=(ArchetypeArchive&&) = delete;

		template <Archetype ArchetypeT>
		void Archiving(Func convFunc)
		{
			constexpr ComponentID archetypeID = QueryComponentID<ArchetypeT>();
			if (archive.find(archetypeID) == archive.end())
			{
				archive[archetypeID] = convFunc;
			}
		}

		template <Archetype ArchetypeT>
		std::optional<Func> Retrive() const
		{
			auto itr = archive.find(QueryComponentID<ArchetypeT>());
			if (itr != archive.end())
			{
				return std::make_optional(itr->second);
			}

			return std::nullopt;
		}

		std::optional<Func> Retrive(ComponentID componentID) const
		{
			auto itr = archive.find(componentID);
			if (itr != archive.end())
			{
				return std::make_optional(itr->second);
			}

			return std::nullopt;
		}

		static ArchetypeArchive& Get()
		{
			static std::unique_ptr<ArchetypeArchive> instance(new ArchetypeArchive);
			return *instance;
		}

	private:
		ArchetypeArchive() = default;

	private:
		std::unordered_map<ComponentID, Func> archive;

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
		ComponentPoolProxy(ComponentPoolProxy&&) = delete;
		ComponentPoolProxy& operator=(const ComponentPoolProxy&) = delete;
		ComponentPoolProxy& operator=(ComponentPoolProxy&&) = delete;

		static ComponentPoolProxy& Get()
		{
			static std::unique_ptr<ComponentPoolProxy> instance(new ComponentPoolProxy);
			return *instance;
		}

		template <Component T>
		void Register(size_t reservedSize = (DEFAULT_COMPONENT_POOL_SIZE))
		{
			constexpr ComponentID componentID = QueryComponentID<T>();
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
			constexpr ComponentID componentID = QueryComponentID<T>();
			return HasComponent(entity, componentID);
		}

		bool HasAllComponents(const Entity entity, const std::vector<ComponentID>& components) const
		{
			for (auto componentID : components)
			{
				auto actualComponent = HasComponent(entity, componentID);
				if (actualComponent == INVALID_COMPONET_ID)
				{
					return false;
				}
			}

			return true;
		}

		template <Component... T>
		bool HasAllComponents(const Entity entity)
		{
			auto initList = { QueryComponentID<T>()... };
			std::vector<ComponentID> components = initList;
			return HasAllComponents(entity, components);
		}

		bool HasAnyComponents(const Entity entity, const std::vector<ComponentID>& components) const
		{
			for (auto componentID : components)
			{
				auto actualComponent = HasComponent(entity, componentID);
				if (actualComponent != INVALID_COMPONET_ID)
				{
					return true;
				}
			}

			return false;
		}

		template <Component... T>
		bool HasAnyComponents(const Entity entity)
		{
			auto initList = { QueryComponentID<T>()... };
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
			constexpr ComponentID componentID = QueryComponentID<T>();
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
			constexpr ComponentID componentID = QueryComponentID<T>();
			return Remove(target, componentID);
		}

		bool Attach(const ComponentID componentID, Entity parent, Entity child)
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
							return true;
						}
					}
				}
			}

			return false;
		}

		template <Component T>
		inline bool Attach(Entity parent, Entity child)
		{
			constexpr ComponentID componentID = QueryComponentID<T>();
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
			constexpr ComponentID componentID = QueryComponentID<T>();
			return Detach(target, componentID);
		}

		template <Component T>
		ComponentPool<T>::OptionalRef QueryComponent(Entity entity)
		{
			constexpr ComponentID targetComponentID = QueryComponentID<T>();
			if (targetComponentID != INVALID_COMPONET_ID)
			{
				ComponentID actualComponentID = HasComponent<T>(entity);

				if (targetComponentID == actualComponentID)
				{
					ComponentPool<T>* componentPool = static_cast<ComponentPool<T>*>(Acquire(targetComponentID));
					if (componentPool != nullptr)
					{
						return componentPool->GetComponent(entity);
					}
				}
				else
				{
					ComponentPoolBase* actualComponentPool = Acquire(actualComponentID);
					if (actualComponentPool != nullptr)
					{
						auto func = ArchetypeArchive<T>::Get().Retrive(actualComponentID);
						if (func)
						{
							return (*func)(actualComponentPool, entity);
						}
					}
				}
			}

			return std::nullopt;
		}

		template <Component T>
		ComponentPool<T>::OptionalConstRef QueryComponent(Entity entity) const
		{
			constexpr ComponentID targetComponentID = QueryComponentID<T>();
			if (targetComponentID != INVALID_COMPONET_ID)
			{
				ComponentID actualComponentID = HasComponent<T>(entity);

				if (targetComponentID == actualComponentID)
				{
					const ComponentPool<T>* componentPool = static_cast<ComponentPool<T>*>(Acquire(targetComponentID));
					if (componentPool != nullptr)
					{
						return componentPool->GetComponent(entity);
					}
				}
				else
				{
					ComponentPoolBase* actualComponentPool = Acquire(actualComponentID);
					if (actualComponentPool != nullptr)
					{
						auto func = ArchetypeArchive<T>::Get().Retrive(actualComponentID);
						if (func)
						{
							auto resOpt = (*func)(actualComponentPool, entity);
							if (resOpt)
							{
								return std::make_optional(std::cref(resOpt->get()));
							}
						}
					}
				}
			}

			return std::nullopt;
		}

		template <Component T>
		std::vector<T> GetComponents()
		{
			ComponentPool<T>& componentPool = Acquire<T>();
			return componentPool->GetComponents();
		}

		template <Component T>
		const std::vector<T>& GetComponents() const
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
				assert(false && "Component does not registered or exist.");
			}

			return itr->second;
		}

		template <Component T>
		ComponentPool<T>& Acquire()
		{
			constexpr ComponentID componentID = QueryComponentID<T>();
			ComponentPool<T>* acquired = static_cast<ComponentPool<T>*>(Acquire(componentID));

			return (*acquired);
		}

		template <Component T>
		const ComponentPool<T>& Acquire() const
		{
			constexpr ComponentID componentID = QueryComponentID<T>();
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
		auto initList = { QueryComponentID<T>()... };
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
		auto initList = { QueryComponentID<T>()... };
		std::vector<ComponentID> componentsList = initList;
		return FilterAllOf(entities, proxy, componentsList);
	}

	template <Component QueryType, Archetype Archetype>
	QueryType& QueryArchetypeData(Archetype& object)
	{
		return std::get<QueryType>(object);
	}

	template <Archetype AT, Component... CT>
	void ArchivingSubcomponents()
	{
		(ArchetypeArchive<CT>::Get().Archiving<AT>([](ComponentPoolBase* poolBase, Entity entity)
			-> std::optional<std::reference_wrapper<CT>>
			{
				if (poolBase != nullptr)
				{
					ComponentPool<AT>* pool = static_cast<ComponentPool<AT>*>(poolBase);
					typename ComponentPool<AT>::OptionalRef archetypeRefOpt = pool->GetComponent(entity);
					if (archetypeRefOpt)
					{
						return std::make_optional(std::ref(std::get<CT>(archetypeRefOpt->get())));
					}
				}

				return std::nullopt;
			}), ...);
	}
}

#define COMPONENT_TYPE_HASH(x) sy::utils::ELFHash(#x)

#define DeclareComponent(ComponentType) \
	struct ComponentType##Registeration \
	{ \
		ComponentType##Registeration() \
		{ \
			auto& proxy = sy::ComponentPoolProxy::Get(); \
			proxy.Register<ComponentType>(); \
		}	\
	private: \
		static ComponentType##Registeration registeration; \
	};\
	template <> \
	constexpr sy::ComponentID sy::QueryComponentID<ComponentType>() \
	{	\
		constexpr uint32_t genID = COMPONENT_TYPE_HASH(ComponentType); \
		static_assert(genID != 0); \
		return static_cast<sy::ComponentID>(genID);	\
	} \

#define RegisterComponent(ComponentType) ComponentType##Registeration ComponentType##Registeration::registeration;

#define DeclareArchetype(ArchetypeType, ...) \
using ArchetypeType = std::tuple<__VA_ARGS__>; \
	struct ArchetypeType##Registeration \
	{ \
		ArchetypeType##Registeration() \
		{ \
			auto& proxy = sy::ComponentPoolProxy::Get(); \
			proxy.Register<ArchetypeType>(); \
			sy::ArchivingSubcomponents<ArchetypeType, __VA_ARGS__>(); \
		}	\
	private: \
		static ArchetypeType##Registeration registeration; \
	};\
	template <> \
	constexpr sy::ComponentID sy::QueryComponentID<ArchetypeType>() \
	{	\
		constexpr uint32_t genID = COMPONENT_TYPE_HASH(ArchetypeType); \
		static_assert(genID != 0); \
		return static_cast<sy::ComponentID>(genID);	\
	} \

#define RegisterArchetype(ArchetypeName) RegisterComponent(ArchetypeName)