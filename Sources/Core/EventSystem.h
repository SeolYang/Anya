#pragma once
#include <PCH.h>
#include <Core/Utility.h>

namespace sy
{
	enum class EventID : uint64_t {};
	constexpr EventID INVALID_EVENT_ID = static_cast<EventID>(0);

	/**
	* @brief	Thread-safe Callback Contrainer (Subscribed Callbacks itself is not a thread safe.)
	*/
	template <typename... EventParams>
	class EventSystem : public std::enable_shared_from_this<EventSystem<EventParams...>>
	{
	public:
		using Callback = std::function<void(EventParams...)>;

		class Event final
		{
			friend EventSystem;
		public:
			Event() = default; // Null Event Object
			Event(const Event&) = delete;
			Event(Event&& rhs) noexcept :
				bIsSubscribed(std::exchange(rhs.bIsSubscribed, false)),
				id(std::exchange(rhs.id, INVALID_EVENT_ID)),
				parentSystem(rhs.parentSystem)
			{
			}

			~Event()
			{
				if (IsAvailable())
				{
					Unsubscribe();
				}
			}

			Event& operator=(const Event&) = delete;
			Event& operator=(Event&& rhs) noexcept
			{
				bIsSubscribed = std::exchange(rhs.bIsSubscribed, false);
				id = std::exchange(rhs.id, INVALID_EVENT_ID);
				parentSystem = rhs.parentSystem;
				return (*this);
			}

			void Unsubscribe()
			{
				if (IsAvailable())
				{
					bIsSubscribed = false;
					parentSystem.lock()->Unsubscribe(id);
				}
			}

			EventID ID() const noexcept { return id; }
			bool IsAvailable() const noexcept { return !parentSystem.expired() && bIsSubscribed && parentSystem.lock()->Contains(id); }
			std::weak_ptr<EventSystem> ParentSystem() const noexcept { return parentSystem; }

		private:
			Event(EventID id, std::weak_ptr<EventSystem> parentSystem) :
				id(id),
				parentSystem(parentSystem),
				bIsSubscribed(true)
			{
			}

		private:
			bool bIsSubscribed = false;
			EventID id = INVALID_EVENT_ID;
			std::weak_ptr<EventSystem> parentSystem;

		};

	public:
		EventSystem() noexcept = default;

		EventSystem(const EventSystem&) = delete;
		EventSystem(EventSystem&&) = delete;
		EventSystem& operator=(const EventSystem&) = delete;
		EventSystem& operator=(EventSystem&&) = delete;

		static auto Create()
		{
			return std::make_shared<EventSystem>();
		}

		/**
		* @brief Becareful with lambda which captured the (this) pointer in object.
		*/
		Event Subscribe(Callback e)
		{
			ReadWriteLock lock{ mutex };
			static auto id = utils::ToUnderlyingType(INVALID_EVENT_ID);
			++id;
			lut[static_cast<EventID>(id)] = std::move(e);

			return Event{ static_cast<EventID>(id), this->shared_from_this() };
		}

		bool Contains(EventID eventID) const noexcept
		{
			ReadOnlyLock lock{ mutex };
			return lut.find(eventID) != lut.end();
		}

		void Notify(EventParams&&... params)
		{
			ReadOnlyLock lock{ mutex };
			for (auto callback : lut)
			{
				callback.second(std::forward<EventParams>(params)...);
			}
		}

		void Unsubscribe(EventID eventID)
		{
			ReadWriteLock lock{ mutex };
			if (eventID != INVALID_EVENT_ID)
			{
				if (auto itr = lut.find(eventID); itr != lut.end())
				{
					lut.erase(itr);
				}
			}
		}

	private:
		mutable Mutex mutex;
		std::unordered_map<EventID, Callback> lut;

	};

	template <typename... EventParams>
	using Event = EventSystem<EventParams...>::Event;
}