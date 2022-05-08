#pragma once
#include <PCH.h>
#include <Core/CommandLineParser.h>
#include <Core/ECS.h>

namespace anya
{
    class Application
    {
    public:
        Application(int32 argc, wchar_t** argv);
        ~Application();

    private:
        CommandLineParser cmdLineParser;
        std::unique_ptr<ecs::ComponentPoolRegistry> componentPoolRegistry;

    };
}