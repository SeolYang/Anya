#pragma once
#include <PCH.h>

namespace sy
{
    class TaskPool
    {
    public:
        TaskPool() :
            bForceExist(false)
        {
            const auto hardwareConcurrency = std::thread::hardware_concurrency() - 1;
            threads.reserve(hardwareConcurrency);
            for (size_t idx = 0; idx < hardwareConcurrency; ++idx)
            {
                threads.emplace_back([this]()
                    {
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

        ~TaskPool()
        {
            bForceExist = true;
            cv.notify_all();

            for (auto& thread : threads)
            {
                thread.join();
            }
        }

        /** ExecuteTask method will excutes 'callables' concurrently. */
        template <typename Callable, typename... Args>
        auto ExecuteTask(Callable&& callable, Args&&... args)
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

        /** AddDeferredTask method will excute 'callables' in single thread as serially.*/
        /** But it is still concurrent to other threads.*/
        /** Must call 'ExecuteDeferredTask' method before use future object that was created from 'AddDeferredTask' method.*/
        template <typename Callable, typename... Args>
        auto AddDeferredTask(Callable&& callable, Args&&... args)
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

        size_t ThreadsCount() const { return threads.size(); }

    private:
        bool bForceExist;
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        std::vector<std::function<void()>> deferredTasks;
        std::condition_variable cv;
        std::mutex mutex;

    };
}