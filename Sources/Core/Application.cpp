#include <PCH.h>
#include <Core/Application.h>
#include <Core/Exceptions.h>
#include <Core/EngineModuleMediator.h>
#include <Render/Renderer.h>
#include <Framework/Scene.h>
#include <Math/Math.h>

namespace sy
{
    LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    Application::Application(std::wstring_view title, int32 argc, wchar_t** argv) :
        componentArchive(ComponentArchive::Instance()),
        title(title),
        cmdLineParser({argc, argv})
    {
        CreateLogger();
        engineModuleMediator = std::make_unique<EngineModuleMediator>(*logger, componentArchive);
        CreateAppWindow();
        renderer = std::make_unique<Renderer>(windowHandle, cmdLineParser);
        LoadScene(EDefaultScenes::Basic);
    }

    Application::~Application()
    {
        scene.reset();
        renderer.reset();
        engineModuleMediator.reset();
        componentArchive.DestroyInstance();

        DestroyAppWindow();
        logger.reset();
    }

    int32 Application::Execute()
    {
        try
        {
            while (!bShouldClose)
            {
                MSG msg;
                ZeroMemory(&msg, sizeof(msg));
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    if (msg.message == WM_QUIT)
                    {
                        bShouldClose = true;
                    }

                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                renderer->Render();
            }
        }
        catch (Exception exception)
        {
            exception.ShowErrorMessageBox();
            return -1;
        }

        return 0;
    }

    void Application::LoadScene(const EDefaultScenes defaultScene)
    {
        if (scene != nullptr)
        {
            scene.reset();
        }

        switch (defaultScene)
        {
        default:
        case EDefaultScenes::Basic:
            logger->info("Loading Basic Scene...");
            scene = std::make_unique<Scene>(componentArchive);
            break;
        }

        scene->Init();
    }

    void Application::CreateLogger()
    {
        const auto currentTime = std::chrono::system_clock::now();
        const auto localTime = std::chrono::current_zone()->to_local(currentTime);
        const std::string fileName = std::format("LOG_{:%F %H %M %S}.log", localTime);

        fs::path logFilePath = "Logs";
        logFilePath /= fileName;
        auto consoleSink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(utils::WStringToAnsi(logFilePath.c_str()), true);

        const auto sinksInitList = { 
            std::static_pointer_cast<spdlog::sinks::sink>(consoleSink),
            std::static_pointer_cast<spdlog::sinks::sink>(fileSink) };
        logger = std::make_unique<spdlog::logger>("Anya", sinksInitList);
        logger->info("Logger Initialized.");
    }

    void Application::CreateAppWindow()
    {
        logger->info("Creating Application window...");
        windowClass = {
            sizeof(WNDCLASSEX),
            CS_CLASSDC,
            WindowProc, 0L, 0L,
            GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
            title.c_str(), NULL};
        RegisterClassEx(&windowClass);

        auto windowStyle = WS_OVERLAPPEDWINDOW;
        windowHandle = CreateWindow(
            windowClass.lpszClassName,
            title.c_str(),
            windowStyle,
            0, 0, 1280, 720,
            NULL, NULL, windowClass.hInstance, NULL);

        ShowWindow(windowHandle, SW_SHOWDEFAULT);
        UpdateWindow(windowHandle);
        logger->info("Application window created.");
    }

    void Application::DestroyAppWindow()
    {
        logger->info("Destroying Application window...");
        DestroyWindow(windowHandle);
        UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
        logger->info("Application window destroyed.");
    }
}