#pragma once
#include <PCH.h>

namespace anya
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

        template <typename Callable, typename... Args>
        auto AddTask(Callable&& callable, Args&&... args)
        {
            using ReturnType = std::invoke_result_t<Callable, Args...>;
            auto task = 
                std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<Callable>(callable), std::forward<Args>(args)...));

            auto future = task->get_future();
            {
                auto lock = std::unique_lock<std::mutex>(mutex);
                tasks.emplace([task]{ (*task)(); });
            }

            cv.notify_one();
            return future;
        }

        size_t ThreadsCount() const { return threads.size(); }

    private:
        bool bForceExist;
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        std::condition_variable cv;
        std::mutex mutex;

    };
}