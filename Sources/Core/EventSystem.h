#pragma once
#include <PCH.h>
#include <Core/Utility.h>

namespace anya
{
	template <typename... EventParams>
	class EventSystem;

	enum class EventID : uint64 {};
	constexpr EventID INVALID_EVENT_ID = static_cast<EventID>(0);

	template <typename... EventParams>
	class EventSystem
	{
	public:
		using Callable = std::function<void(EventParams...)>;

		class Event final
		{
			friend EventSystem;
		public:
			Event() = default; // Null Event Object
			Event(const Event&) = delete;
			Event(Event&& rhs) noexcept :
				bIsSubscribed(std::exchange(rhs.bIsSubscribed, false)),
				id(std::exchange(rhs.id, INVALID_EVENT_ID)),
				parentSystem(std::exchange(rhs.parentSystem, nullptr))
			{
			}

			~Event()
			{
				Unsubscribe();
			}

			Event& operator=(const Event&) = delete;
			Event& operator=(Event&& rhs) noexcept
			{
				bIsSubscribed = std::exchange(rhs.bIsSubscribed, false);
				id = std::exchange(rhs.id, INVALID_EVENT_ID);
				parentSystem = std::exchange(rhs.parentSystem, nullptr);
				return (*this);
			}

			void Unsubscribe()
			{
				if (parentSystem != nullptr)
				{
					bIsSubscribed = false;
					parentSystem->Unsubscribe(id);
				}
			}

			EventID ID() const { return id; }
			bool IsAvailable() const { return parentSystem != nullptr && bIsSubscribed && parentSystem.Contains(id); }

			EventSystem& ParentSystem() const { return parentSystem; }

		private:
			Event(EventID id, EventSystem* parentSystem) :
				id(id),
				parentSystem(parentSystem),
				bIsSubscribed(true)
			{
			}

		private:
			bool bIsSubscribed = false;
			EventID id = INVALID_EVENT_ID;
			EventSystem* parentSystem = nullptr;

		};

	public:
		/** Becareful with lambda which capture the (this) pointer in object. */
		Event Subscribe(Callable e)
		{
			static auto id = utils::ToUnderlyingType(INVALID_EVENT_ID);
			lut[++id] = std::move(e);

			return Event{ static_cast<EventID>(id), this };
		}

		bool Contains(EventID eventID) const
		{
			return lut.find(eventID) != lut.end();
		}

		void Notify(EventParams&&... params)
		{
			for (auto callable : lut)
			{
				callable.second(std::forward<EventParams>(params)...);
			}
		}

		void Unsubscribe(EventID eventID)
		{
			if (eventID != INVALID_EVENT_ID)
			{
				if (auto itr = lut.find(eventID); itr != lut.end())
				{
					lut.erase(itr);
				}
			}
		}

	private:
		std::unordered_map<EventID, Callable> lut;

	};

	template <typename... EventParams>
	using Event = EventSystem<EventParams...>::Event;
}