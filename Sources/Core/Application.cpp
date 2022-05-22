#include <PCH.h>
#include <Core/Application.h>
#include <Core/Exceptions.h>
#include <Math/Math.h>

namespace sy
{
    Application::Application(std::wstring_view title, int32 argc, wchar_t** argv) :
        title(title),
        cmdLineParser({argc, argv})
    {
    }

    Application::~Application()
    {
    }

    int32 Application::Execute()
    {
        try
        {
        }
        catch (Exception exception)
        {
            exception.ShowErrorMessageBox();
            return -1;
        }

        return 0;
    }
}