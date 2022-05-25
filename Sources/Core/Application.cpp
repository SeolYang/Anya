#include <PCH.h>
#include <Core/Application.h>
#include <Core/Exceptions.h>
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
        title(title),
        cmdLineParser({argc, argv})
    {
        CreateAppWindow();
        renderer = std::make_unique<Renderer>(windowHandle, cmdLineParser);
        
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

    void Application::CreateAppWindow()
    {
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
    }

    void Application::DestroyAppWindow()
    {
        DestroyWindow(windowHandle);
        UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
    }
}