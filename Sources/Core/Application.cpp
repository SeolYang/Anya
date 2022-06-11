#include <PCH.h>
#include <Core/Application.h>
#include <Core/Exceptions.h>
#include <Core/TaskManager.h>
#include <Core/Timer.h>
#include <Core/PerformanceMonitor.h>
#include <Core/FrameCounter.h>
#include <Core/EngineCoreMediator.h>
#include <UI/UIContext.h>
#include <UI/ApplicationMenuBar.h>
#include <Rendering/RenderContext.h>
#include <Framework/Scene.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace sy
{
    LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        {
            return true;
        }

        switch (msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            break;
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
        uiContext.reset();
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

                    uiContext->BeginFrame();
                    {
                        menuBarUI->OnGUI();
                        if (bShowDemoWindow)
                        {
                            ImGui::ShowDemoWindow(&bShowDemoWindow);
                        }
                        uiContext->Render();
                    }
                    uiContext->EndFrame();

                    renderContext->BeginFrame();
                    {
                        renderContext->Render();
                    }
                    renderContext->EndFrame();
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
        CreateUI();
        renderContext = std::make_unique<RenderContext>(windowHandle, cmdLineParser);
        LoadScene(EDefaultScenes::Basic);

    }

    void Application::CreateUI()
    {
        uiContext = std::make_unique<UIContext>();
        menuBarUI = std::make_unique<ApplicationMenuBar>(*this);
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
        constexpr auto WindowStyle = WS_POPUP;
        constexpr auto ExWindowStyle = WS_EX_APPWINDOW;

        RECT winRect{
            0,
            static_cast<LONG>(resolution.Height),
            static_cast<LONG>(resolution.Width),
            0
        };

        ANYA_ASSERT(AdjustWindowRectEx(&winRect, WindowStyle, false, ExWindowStyle) != 0, "Failed to adjust window rect");

        windowHandle = CreateWindowEx(
            ExWindowStyle,
            windowClass.lpszClassName,
            title.c_str(),
            WindowStyle,
            (screenWidth - resolution.Width)/2, (screenHeight - resolution.Height)/2,
            winRect.right - winRect.left, winRect.top - winRect.bottom,
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