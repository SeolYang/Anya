#pragma once
#include <cstdint>
#include <optional>
#include <utility>
#include <random>
#include <thread>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <atomic>

namespace sy::ecs
{
	using Entity = uint64_t;
	constexpr Entity INVALID_ENTITY_HANDLE = 0;
	constexpr size_t DEFAULT_COMPONENT_POOL_SIZE = 16;

	constexpr bool USE_RANDOM_NUM_FOR_ENTITY_HANDLE = false;

	inline Entity GenerateEntity()
	{
		if (USE_RANDOM_NUM_FOR_ENTITY_HANDLE)
		{
			static thread_local std::mt19937_64 generator(
				std::hash<std::thread::id>{}(std::this_thread::get_id()));

			std::uniform_int_distribution<Entity> dist(std::numeric_limits<Entity>::min() + 1, std::numeric_limits<Entity>::max());
			return dist(generator);
		}

		static std::atomic<Entity> handle = 1;
		return handle++;
	}

	using ComponentID = size_t;
	class ComponentIDGenerator
	{
	public:
		ComponentIDGenerator() = delete;

		template <typename Component>
		inline static ComponentID Value()
		{
			return InternalValue<Component>();
		}

		template <typename Component>
		inline static ComponentID Value(const Component&)
		{
			return InternalValue<Component>();
		}

	private:
		template <typename Component>
		inline static ComponentID InternalValue()
		{
			static_assert(std::same_as<std::decay_t<Component>, Component>);
			const static ComponentID internalID = idCounter++;
			return internalID;
		}

	private:
		inline static size_t idCounter{};

	};

	struct ComponentInfo
	{
		ComponentID ID;
		std::string Name;
		size_t Size;
		size_t Alignment;
	};

	template <typename Component>
	class ComponentInfoGenerator
	{
	public:
		ComponentInfoGenerator() = delete;

		static ComponentInfo Generate()
		{
			return ComponentInfo{
				.ID = ComponentIDGenerator::Value<Component>(),
				.Name = typeid(Component).name(),
				.Size = sizeof(Component),
				.Alignment = alignof(Component)
			};
		}
	};

	class ComponentInfoRegistry
	{
	public:
		ComponentInfoRegistry() = delete;

		static std::optional<ComponentInfo> Acquire(ComponentID id)
		{
			std::optional<ComponentInfo> res = std::nullopt;
			auto itr = lut.find(id);
			if (itr != lut.end())
			{
				res = std::make_optional(itr->second);
			}

			return res;
		}

		template <typename Component>
		static ComponentInfo Acquire()
		{
			return InternalAcquire<Component>();
		}

	private:
		template <typename Component>
		static ComponentInfo InternalAcquire()
		{
			ComponentID id = ComponentIDGenerator::Value<Component>();
			if (lut.find(id) == lut.end())
			{
				lut[id] = ComponentInfoGenerator<Component>::Generate();
			}

			return lut[id];
		}

	private:
		inline static std::unordered_map<ComponentID, ComponentInfo> lut{};

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

		ComponentInfo AcquireComponentInfo() const
		{
			return componentInfo;
		}

		virtual bool Contains(Entity entity) const = 0;
		virtual bool Create(Entity entity) = 0;
		virtual void Remove(Entity entity) = 0;
		virtual size_t Size() const = 0;
		virtual void Clear() = 0;

	private:
		ComponentInfo componentInfo;

	};

	template <typename Component>
	class ComponentPoolImpl : public ComponentPoolBase
	{
	public:
		using RefWrapper = std::reference_wrapper<Component>;
		using ConstRefWrapper = std::reference_wrapper<const Component>;
		using ComponentRetType = std::optional<RefWrapper>;
		using ConstComponentRetType = std::optional<ConstRefWrapper>;

	public:
		explicit ComponentPoolImpl(size_t reservedSize) :
			ComponentPoolBase(sy::ecs::ComponentInfoRegistry::Acquire<Component>())
		{
			components.reserve(reservedSize);
			entities.reserve(reservedSize);
			lut.reserve(reservedSize);
		}

		Component& operator[](size_t idx)
		{
			return components[idx];
		}

		const Component& operator[](size_t idx) const
		{
			return components[idx];
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

		ComponentRetType CreateWithReturn(Entity entity)
		{
			if (bool bIsValidEntityHandle = entity != INVALID_ENTITY_HANDLE; bIsValidEntityHandle)
			{
				if (Create(entity))
				{
					return (components.back());
				}
			}

			return std::nullopt;
		}

		ConstComponentRetType GetComponent(Entity entity) const
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

		ComponentRetType GetComponent(Entity entity)
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

		const std::vector<Component>& GetComponents() const
		{
			return components;
		}

		std::vector<Component>& GetComponents()
		{
			return components;
		}

		std::optional<Entity> GetEntity(size_t componentIdx) const
		{
			return (componentIdx < entities.size()) ?
				std::optional<Entity>(entities[componentIdx]) :
				std::nullopt;
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

		inline bool CheckRelationBetween(Entity entity, const Component& component) const
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

		inline bool CheckValidationOf(const Component& component) const
		{
			auto foundItr = std::find_if(components.begin(), components.end(), [&](const Component& validComponent)
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

		inline void MoveElementBlockFrom(std::vector<Component> moveComponents, std::vector<Entity> moveEntities, size_t moveAt)
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
		std::vector<Component> components;
		/* Entities[idx] == Entities[LUT[Entities[idx]]] **/
		std::vector<Entity> entities; 
		std::unordered_map<Entity, size_t> lut;

	};

	template <typename Component>
	class ComponentPool : public ComponentPoolImpl<Component>
	{
	public:
		explicit ComponentPool(size_t reservedSize = (DEFAULT_COMPONENT_POOL_SIZE)) :
			ComponentPoolImpl<Component>(reservedSize)
		{
		}
	};

	class ComponentPoolRegistry
	{
	public:
		ComponentPoolRegistry() = default;
		~ComponentPoolRegistry()
		{
			for (auto registered : registry)
			{
				delete registered.second;
			}
		}

		ComponentPoolRegistry(const ComponentPoolRegistry&) = delete;
		ComponentPoolRegistry& operator=(const ComponentPoolRegistry&) = delete;

		ComponentPoolRegistry(ComponentPoolRegistry&& rhs) noexcept :
			registry(std::move(rhs.registry))
		{
		}

		template <typename Component>
		ComponentPool<Component>* AcquireWithComponentType()
		{
			const ComponentID componentID = ComponentIDGenerator::Value<Component>();
			return static_cast<ComponentPool<Component>*>(Acquire(componentID));
		}

		ComponentPoolBase* Acquire(const ComponentID componentID)
		{
			auto itr = registry.find(componentID);
			if (itr != registry.end())
			{
				return itr->second;
			}

			return nullptr;
		}

		template <typename Component>
		void Register(size_t reservedSize = (DEFAULT_COMPONENT_POOL_SIZE))
		{
			const ComponentID componentID = ComponentIDGenerator::Value<Component>();
			if (registry.find(componentID) == registry.end())
			{
				registry[componentID] = new ComponentPool<Component>(reservedSize);
			}
		}

		// 매크로와 singleton 패턴을 통해 모든 컴포넌트 풀을 등록한다음.
		// 엔진에서 싱글턴 객체를 사용하게 하고, 이를 완전히 무효화 시키도록 한다.
		static ComponentPoolRegistry* GetGlobalInitRegistry(bool makeExpire = false)
		{
			static bool expired = false;
			static ComponentPoolRegistry* instance = nullptr;
			if (!expired)
			{
				if (instance == nullptr)
				{
					instance = new ComponentPoolRegistry();
				}

				expired = makeExpire;
				return instance;
			}

			return nullptr;
		}

	private:
		std::unordered_map<ComponentID, ComponentPoolBase*> registry;

	};
}

#define DeclareComponent(ComponentType) \
	struct ComponentType##Registeration \
	{ \
		ComponentType##Registeration() \
		{ \
			auto registry = sy::ecs::ComponentPoolRegistry::GetGlobalInitRegistry(); \
			if (registry != nullptr) \
			{ \
				registry->Register<ComponentType>(); \
			} \
		}	\
	private: \
		static ComponentType##Registeration registeration; \
	};

#define RegisterComponent(ComponentType) ComponentType##Registeration ComponentType##Registeration::registeration;