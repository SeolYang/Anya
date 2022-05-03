#include <PCH.h>
#include <Core/Application.h>

namespace anya
{
    Application::Application(int argc, char** argv) :
        cmdLineParser({argc, argv}),
        componentPoolRegistry(std::unique_ptr<ComponentPoolRegistry>(ComponentPoolRegistry::GetGlobalInitRegistry(true)))
    {
    }

    Application::~Application()
    {
    }
}