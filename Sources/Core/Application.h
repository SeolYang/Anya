#pragma once
#include <PCH.h>
#include <Core/CommandLineParser.h>
#include <Core/ECS.h>
#include <Render/Renderer.h>

namespace sy
{
    enum class EDefaultScenes
    {
        Basic, /* Empty Scene */
    };

    class Scene;
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

        std::unique_ptr<spdlog::logger> logger;
        CommandLineParser cmdLineParser;
        ComponentArchive& componentArchive;
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<Scene> scene;

    };
}