#pragma once
#include <PCH.h>
#include <Core/CommandLineParser.h>

namespace sy
{
    enum class EDefaultScenes
    {
        Basic, /* Empty Scene */
    };

    class TaskPool;
    class Scene;
    class RenderContext;
    class ComponentArchive;
    class EngineCore;
    class Timer;
    class PerformanceMonitor;
    class Application
    {
    public:
        Application(std::wstring_view title, int32 argc, wchar_t** argv) noexcept(false);
        virtual ~Application();

        Application(const Application&) = delete;
        Application(Application&&) = delete;

        int32 Execute();

        [[nodiscard]] std::wstring_view Title() const noexcept { return title; }

        inline void Close() { bShouldClose = true; }

        void LoadScene(const EDefaultScenes defaultScene);

    private:
        void Initialize();
        void CreateLogger();
        void CreateAppWindow();
        void DestroyAppWindow();

    private:
        bool bShouldClose = false;
        HWND windowHandle;
        WNDCLASSEX windowClass;
        std::wstring title;

        CommandLineParser cmdLineParser;
        std::unique_ptr<TaskPool> taskPool;
        std::unique_ptr<spdlog::logger> logger;
        std::unique_ptr<Timer> mainTimer;
        std::unique_ptr<PerformanceMonitor> perfMonitor;
        ComponentArchive& componentArchive;
        std::unique_ptr<EngineCore> engineModuleMediator;
        std::unique_ptr<RenderContext> renderContext;
        std::unique_ptr<Scene> scene;


    };
}