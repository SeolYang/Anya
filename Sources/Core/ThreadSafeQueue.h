#pragma once
#include <PCH.h>

namespace sy
{
    template <typename T>
    class ThreadSafeQueue
    {
    public:
        ThreadSafeQueue() noexcept = default;
        ThreadSafeQueue(const ThreadSafeQueue& rhs) noexcept
        {
            std::unique_lock<std::mutex> lock(mutex);
            queue = rhs.queue;
        }

        ThreadSafeQueue& operator=(const ThreadSafeQueue& rhs) noexcept
        {
            ReadWriteLock lock(mutex);
            queue = rhs.queue;
            return (*this);
        }

        void Push(T&& value)
        {
            ReadWriteLock lock(mutex);
            queue.push(std::forward<T>(value));
        }

        bool TryPop(T& result)
        {
            ReadWriteLock lock(mutex);
            if (queue.empty())
            {
                return false;
            }

            result = std::move(queue.front());
            queue.pop();
            return true;
        }

        bool IsEmpty() const
        {
            ReadOnlyLock lock(mutex);
            return queue.empty();
        }

        size_t Size() const
        {
            ReadOnlyLock lock(mutex);
            return queue.size();
        }

    private:
        Mutex mutex;
        std::queue<T> queue;

    };
}
