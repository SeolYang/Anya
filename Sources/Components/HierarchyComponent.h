#pragma once
#include <Core/ECS.h>

namespace sy
{
	struct HierarchyComponent
	{
		Entity parentEntity = INVALID_ENTITY_HANDLE;
	};

	DeclareComponent(HierarchyComponent);
}

namespace sy
{
	template <>
	class ComponentPool<HierarchyComponent> :
		public ComponentPoolImpl<HierarchyComponent>
	{
	public:
		explicit ComponentPool(size_t reservedSize = DEFAULT_COMPONENT_POOL_SIZE) :
			ComponentPoolImpl<HierarchyComponent>(reservedSize)
		{
		}

		virtual void Attach(Entity parent, Entity child) override
		{
			auto lutParentItr = lut.find(parent);
			auto lutChildItr = lut.find(child);

			bool bIsExistEntities = lutParentItr != lut.end() && lutChildItr != lut.end();
			bool bIsNotSameEntities = parent != child;
			if (bIsExistEntities && bIsNotSameEntities)
			{
				size_t parentOffset = lutParentItr->second;
				size_t childOffset = lutChildItr->second;

				if (components[childOffset].parentEntity != parent)
				{
					std::vector<HierarchyComponent> tempComps;
					std::vector<Entity> tempEntities;
					InjectSubtree(parent, childOffset, tempComps, tempEntities);

					size_t moveBegin = 0;
					size_t moveEnd = 0;
					size_t moveTo = 0;
					size_t moveAt = 0;
					{
						if (childOffset < parentOffset)
						{
							moveBegin = childOffset + tempComps.size();
							moveTo = childOffset;

							for (moveEnd = moveBegin; moveEnd < components.size(); ++moveEnd)
							{
								if (components[moveEnd].parentEntity == INVALID_ENTITY_HANDLE)
								{
									break;
								}
							}
							++moveEnd;

							moveAt = moveEnd - 1;
						}
						else if (childOffset > parentOffset)
						{
							moveBegin = parentOffset + 1;
							moveEnd = moveBegin + (childOffset - parentOffset - 1);
							moveTo = moveBegin + tempComps.size();

							moveAt = moveBegin;
						}

						MoveElementBlock(moveBegin, moveEnd, moveTo);
						MoveElementBlockFrom(std::move(tempComps), std::move(tempEntities), moveAt);
						UpdateLUT();
					}
				}
			}
		}

		virtual void Detach(Entity target) override
		{
			if (auto targetItr = lut.find(target); targetItr != lut.end())
			{
				size_t rootIdx = targetItr->second;

				if (components[rootIdx].parentEntity != INVALID_ENTITY_HANDLE)
				{
					std::vector<HierarchyComponent> componentSubTree;
					std::vector<Entity> entitySubTree;
					InjectSubtree(INVALID_ENTITY_HANDLE, rootIdx, componentSubTree, entitySubTree);

					size_t injectedRange = entitySubTree.size();
					MoveElementBlock(rootIdx + injectedRange, Size(), rootIdx);
					MoveElementBlockFrom(std::move(componentSubTree), std::move(entitySubTree), rootIdx + injectedRange);
					UpdateLUT();
				}
			}
		}

		virtual void Remove(Entity target) override
		{
			if (auto targetItr = lut.find(target); targetItr != lut.end())
			{
				size_t rootIdx = targetItr->second;

				std::vector<HierarchyComponent> componentSubTree;
				std::vector<Entity> entitySubTree;
				InjectSubtree(INVALID_ENTITY_HANDLE, rootIdx, componentSubTree, entitySubTree);

				size_t injectedRange = entitySubTree.size();
				MoveElementBlock(rootIdx + injectedRange, Size(), rootIdx);

				size_t newSize = Size() - injectedRange;
				components.resize(newSize);
				entities.resize(newSize);

				for (auto injectedEntity : entitySubTree)
				{
					lut.erase(injectedEntity);
				}
			}
		}

	private:
		size_t InjectSubtree(Entity parent, size_t rootIdx, std::vector<HierarchyComponent>& tempComps, std::vector<Entity>& tempEntities)
		{
			if (rootIdx >= 0 && rootIdx < components.size())
			{
				Entity rootEntity = entities[rootIdx];
				entities[rootIdx] = INVALID_ENTITY_HANDLE;

				components[rootIdx].parentEntity = parent;
				tempComps.emplace_back(components[rootIdx]);
				components[rootIdx].parentEntity = INVALID_ENTITY_HANDLE;
				tempEntities.emplace_back(rootEntity);

				size_t numSubTreeElements = 1;
				for (size_t childIdx = rootIdx + 1; childIdx < components.size(); ++childIdx)
				{
					if (components[childIdx].parentEntity == rootEntity)
					{
						numSubTreeElements += InjectSubtree(rootEntity, childIdx, tempComps, tempEntities);
						childIdx = rootIdx + numSubTreeElements - 1;
					}
					else
					{
						break;
					}
				}

				return numSubTreeElements;
			}

			return 0;
		}

	};

	template <typename ArchetypeT>
	concept HierarchyArchetype = ArchetypeContains<ArchetypeT, HierarchyComponent>;

	template <HierarchyArchetype T>
	class ComponentPool<T> : public ComponentPoolImpl<T>
	{
	public:
		explicit ComponentPool(size_t reservedSize = DEFAULT_COMPONENT_POOL_SIZE) :
			ComponentPoolImpl<T>(reservedSize)
		{
		}

		virtual void Attach(Entity parent, Entity child) override
		{
			auto& lut = this->lut;
			auto& components = this->components;

			auto lutParentItr = lut.find(parent);
			auto lutChildItr = lut.find(child);

			bool bIsExistEntities = lutParentItr != lut.end() && lutChildItr != lut.end();
			bool bIsNotSameEntities = parent != child;
			if (bIsExistEntities && bIsNotSameEntities)
			{
				size_t parentOffset = lutParentItr->second;
				size_t childOffset = lutChildItr->second;

				if (std::get<HierarchyComponent>(components[childOffset]).parentEntity != parent)
				{
					std::vector<T> tempComps;
					std::vector<Entity> tempEntities;
					InjectSubtree(parent, childOffset, tempComps, tempEntities);

					size_t moveBegin = 0;
					size_t moveEnd = 0;
					size_t moveTo = 0;
					size_t moveAt = 0;
					{
						if (childOffset < parentOffset)
						{
							moveBegin = childOffset + tempComps.size();
							moveTo = childOffset;

							for (moveEnd = moveBegin; moveEnd < components.size(); ++moveEnd)
							{
								if (std::get<HierarchyComponent>(components[moveEnd]).parentEntity == INVALID_ENTITY_HANDLE)
								{
									break;
								}
							}
							++moveEnd;

							moveAt = moveEnd - 1;
						}
						else if (childOffset > parentOffset)
						{
							moveBegin = parentOffset + 1;
							moveEnd = moveBegin + (childOffset - parentOffset - 1);
							moveTo = moveBegin + tempComps.size();

							moveAt = moveBegin;
						}

						this->MoveElementBlock(moveBegin, moveEnd, moveTo);
						this->MoveElementBlockFrom(std::move(tempComps), std::move(tempEntities), moveAt);
						this->UpdateLUT();
					}
				}
			}
		}

		virtual void Detach(Entity target) override
		{
			auto& lut = this->lut;
			if (auto targetItr = lut.find(target); targetItr != lut.end())
			{
				size_t rootIdx = targetItr->second;

				if (std::get<HierarchyComponent>(this->components[rootIdx]).parentEntity != INVALID_ENTITY_HANDLE)
				{
					std::vector<T> componentSubTree;
					std::vector<Entity> entitySubTree;
					this->InjectSubtree(INVALID_ENTITY_HANDLE, rootIdx, componentSubTree, entitySubTree);

					size_t injectedRange = entitySubTree.size();
					this->MoveElementBlock(rootIdx + injectedRange, this->Size(), rootIdx);
					this->MoveElementBlockFrom(std::move(componentSubTree), std::move(entitySubTree), rootIdx + injectedRange);
					this->UpdateLUT();
				}
			}
		}
		
		virtual void Remove(Entity target) override
		{
			auto& lut = this->lut;
			if (auto targetItr = lut.find(target); targetItr != lut.end())
			{
				size_t rootIdx = targetItr->second;

				std::vector<T> componentSubTree;
				std::vector<Entity> entitySubTree;
				this->InjectSubtree(INVALID_ENTITY_HANDLE, rootIdx, componentSubTree, entitySubTree);

				size_t injectedRange = entitySubTree.size();
				this->MoveElementBlock(rootIdx + injectedRange, this->Size(), rootIdx);

				size_t newSize = this->Size() - injectedRange;
				this->components.resize(newSize);
				this->entities.resize(newSize);

				for (auto injectedEntity : entitySubTree)
				{
					lut.erase(injectedEntity);
				}
			}
		}

	private:
		size_t InjectSubtree(Entity parent, size_t rootIdx, std::vector<T>& tempComps, std::vector<Entity>& tempEntities)
		{
			auto& components = this->components;
			if (rootIdx >= 0 && rootIdx < components.size())
			{
				auto& entities = this->entities;
				Entity rootEntity = entities[rootIdx];
				entities[rootIdx] = INVALID_ENTITY_HANDLE;

				std::get<HierarchyComponent>(components[rootIdx]).parentEntity = parent;
				tempComps.emplace_back(components[rootIdx]);
				std::get<HierarchyComponent>(components[rootIdx]).parentEntity = INVALID_ENTITY_HANDLE;
				tempEntities.emplace_back(rootEntity);

				size_t numSubTreeElements = 1;
				for (size_t childIdx = rootIdx + 1; childIdx < components.size(); ++childIdx)
				{
					if (std::get<HierarchyComponent>(components[childIdx]).parentEntity == rootEntity)
					{
						numSubTreeElements += this->InjectSubtree(rootEntity, childIdx, tempComps, tempEntities);
						childIdx = rootIdx + numSubTreeElements - 1;
					}
					else
					{
						break;
					}
				}

				return numSubTreeElements;
			}

			return 0;
		}

	};
}
