#include <PCH.h>
#include <Core/Application.h>
#include <Core/Exceptions.h>
#include <Core/TaskManager.h>
#include <Core/Timer.h>
#include <Core/PerformanceMonitor.h>
#include <Core/FrameCounter.h>
#include <Core/EngineCoreMediator.h>
#include <Rendering/RenderContext.h>
#include <Framework/Scene.h>

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
        cmdLineParser({argc, argv}),
        windowHandle(NULL)
    {
    }

    Application::~Application()
    {
        scene.reset();
        renderContext.reset();
        engineModuleMediator.reset();
        componentArchive.DestroyInstance();

        DestroyAppWindow();
        logger.reset();
        mainTimer.reset();
        perfMonitor.reset();
    }

    int32 Application::Execute()
    {
        try
        {
            Initialize();

            FrameCounter frameCounter;
            while (!bShouldClose)
            {
                mainTimer->Begin();
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

                    // For frame limit
                    //auto end = std::chrono::high_resolution_clock::now() + chrono::duration_cast<chrono::nanoseconds>(chrono::milliseconds(16));
                    //while (std::chrono::high_resolution_clock::now() < end)
                    //{
                    //    std::this_thread::yield();
                    //}

                    renderContext->Render();
                }
                mainTimer->End();
 
                frameCounter.Update(mainTimer->GetDeltaTimeNanos());
                perfMonitor->UpdateAs(TEXT("MainLoopDelta"), mainTimer->GetDeltaTimeNanos());
            }
        }
        catch (Exception& exception)
        {
            logger->error(utils::WStringToAnsi(exception.GetMessageW()));
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

    void Application::Initialize()
    {
        taskManager = std::make_unique<TaskManager>();
        CreateLogger();
        mainTimer = std::make_unique<Timer>();
        perfMonitor = std::make_unique<PerformanceMonitor>();
        engineModuleMediator = std::make_unique<EngineCore>(*taskManager, *mainTimer, *perfMonitor, *logger, componentArchive);
        CreateAppWindow();
        renderContext = std::make_unique<RenderContext>(windowHandle, cmdLineParser);
        LoadScene(EDefaultScenes::Basic);

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
        logger = std::make_unique<spdlog::logger>("Core", sinksInitList);
        logger->info("Logger Initialized.");
    }

    void Application::CreateAppWindow()
    {
        logger->info("Creating Application window...");
        windowClass = {
            sizeof(WNDCLASSEX),
            CS_OWNDC,
            WindowProc, 0L, 0L,
            GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL,
            title.c_str(), NULL};
        RegisterClassEx(&windowClass);

        const auto screenWidth = GetSystemMetrics(SM_CXSCREEN);
        const auto screenHeight = GetSystemMetrics(SM_CYSCREEN);

        const Dimensions resolution{ 1280, 720 };

        auto error = GetLastError();
        auto windowStyle = WS_OVERLAPPEDWINDOW;
        windowHandle = CreateWindowEx(
            NULL,
            windowClass.lpszClassName,
            title.c_str(),
            windowStyle,
            (screenWidth - resolution.Width)/2, (screenHeight - resolution.Height)/2,
            resolution.Width, resolution.Height,
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