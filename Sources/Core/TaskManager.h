#pragma once
#include <PCH.h>

namespace sy
{
    class TaskManager
    {
    public:
        TaskManager() :
            bForceExist(false)
        {
            const auto hardwareConcurrency = std::thread::hardware_concurrency() - 1;
            threads.reserve(hardwareConcurrency);

            tidx = 0; /** Index 0 for main thread. */
            for (size_t idx = 0; idx < hardwareConcurrency; ++idx)
            {
                threads.emplace_back([this, idx]()
                    {
                        tidx = idx + 1;
                        while (true)
                        {
                            std::function<void()> task;
                            {
                                auto lock = std::unique_lock<std::mutex>(mutex);
                                cv.wait(lock, [this]
                                    {
                                        return bForceExist || !tasks.empty();
                                    });

                                if (bForceExist)
                                {
                                    return;
                                }

                                task = std::move(tasks.front());
                                tasks.pop();
                            }

                            task();
                        }
                    });
            }
        }

        ~TaskManager()
        {
            bForceExist = true;
            cv.notify_all();

            for (auto& thread : threads)
            {
                thread.join();
            }
        }

        /**
        * @brief ExecuteTask method will excutes 'callables' concurrently.
        */
        template <typename Callable, typename... Args>
        [[nodiscard]] auto ExecuteTask(Callable&& callable, Args&&... args)
        {
            using ReturnType = std::invoke_result_t<Callable, Args...>;
            auto task =
                std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<Callable>(callable), std::forward<Args>(args)...));

            auto future = task->get_future();
            {
                auto lock = std::unique_lock<std::mutex>(mutex);
                tasks.emplace([task] { (*task)(); });
            }

            cv.notify_one();
            return future;
        }

        /**
        * @brief AddDeferredTask method will excute 'callables' in single thread as serially. But it is still concurrent to other threads.
        * And also must call 'ExecuteDeferredTask' method before use future object that was created from 'AddDeferredTask' method.
        */
        template <typename Callable, typename... Args>
        [[nodiscard]] auto AddDeferredTask(Callable&& callable, Args&&... args)
        {
            using ReturnType = std::invoke_result_t<Callable, Args...>;
            auto task =
                std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<Callable>(callable), std::forward<Args>(args)...));

            auto future = task->get_future();
            deferredTasks.emplace_back([task] { (*task)(); });

            return future;
        }

        void ExecuteDeferredTask()
        {
            if (!deferredTasks.empty())
            {
                auto lock = std::unique_lock<std::mutex>(mutex);
                {
                    tasks.emplace([deferredTasks = std::move(deferredTasks)]
                        {
                            for (auto& task : deferredTasks)
                            {
                                task();
                            }
                        });
                    lock.unlock();
                }

                cv.notify_one();
            }
        }

        [[nodiscard]] size_t GetNumWorkerThreads() const { return threads.size() + 1; } // one for main thread

        [[nodiscard]] static size_t GetCurrentWorkerThreadIndex()
        {
            return tidx;
        }

        [[nodiscard]] static bool IsWorkerThread()
        {
            return tidx != InvalidThreadIndex;
        }

    public:
        static constexpr size_t InvalidThreadIndex = 0xffffffffffffffff;

    private:
        bool bForceExist;
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        std::vector<std::function<void()>> deferredTasks;
        std::condition_variable cv;
        std::mutex mutex;

        static inline thread_local size_t tidx = std::numeric_limits<size_t>::max();

    };
}