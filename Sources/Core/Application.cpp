#include <PCH.h>
#include <Core/Application.h>
#include <Math/Math.h>

namespace anya
{
    Application::Application(int32 argc, wchar_t** argv) :
        cmdLineParser({argc, argv}),
        componentPoolRegistry(std::unique_ptr<ecs::ComponentPoolRegistry>(ecs::ComponentPoolRegistry::GetGlobalInitRegistry(true)))
    {
    }

    Application::~Application()
    {
    }
}