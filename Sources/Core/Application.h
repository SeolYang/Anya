#pragma once
#include <PCH.h>
#include <Core/CommandLineParser.h>

namespace sy
{
    enum class EDefaultScenes
    {
        Basic, /* Empty Scene */
    };

    class Scene;
    class Renderer;
    class ComponentArchive;
    class EngineModuleMediator;
    class Application
    {
    public:
        Application(std::wstring_view title, int32 argc, wchar_t** argv);
        virtual ~Application();

        Application(const Application&) = delete;
        Application(Application&&) = delete;

        int32 Execute();

        std::wstring_view Title() const noexcept { return title; }

        inline void Close() { bShouldClose = true; }

        void LoadScene(const EDefaultScenes defaultScene);

    private:
        void CreateLogger();
        void CreateAppWindow();
        void DestroyAppWindow();

    private:
        bool bShouldClose = false;
        HWND windowHandle;
        WNDCLASSEX windowClass;
        std::wstring title;

        CommandLineParser cmdLineParser;
        std::unique_ptr<spdlog::logger> logger;
        ComponentArchive& componentArchive;
        std::unique_ptr<EngineModuleMediator> engineModuleMediator;
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<Scene> scene;

    };
}