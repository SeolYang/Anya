#pragma once
#include <PCH.h>
#include <Core/CommandLineParser.h>
#include <Core/ECS.h>
#include <Render/Renderer.h>

namespace sy
{
    class Application
    {
    public:
        Application(std::wstring_view title, int32 argc, wchar_t** argv);
        virtual ~Application() = default;

        Application(const Application&) = delete;
        Application(Application&&) = delete;

        int32 Execute();

        std::wstring_view Title() const noexcept { return title; }

        inline void Close() { bShouldClose = true; }

    private:
        void CreateAppWindow();
        void DestroyAppWindow();

    private:
        bool bShouldClose = false;
        HWND windowHandle;
        WNDCLASSEX windowClass;
        std::wstring title;

        CommandLineParser cmdLineParser;
        std::unique_ptr<Renderer> renderer;

    };
}