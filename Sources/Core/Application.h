#pragma once
#include <PCH.h>
#include <Core/CommandLineParser.h>
#include <Core/ECS.h>

namespace anya
{
    using ComponentPoolRegistry = sy::ecs::ComponentPoolRegistry;
    class Application
    {
    public:
        Application(int argc, char** argv);
        ~Application();

    private:
        CommandLineParser cmdLineParser;
        std::unique_ptr<ComponentPoolRegistry> componentPoolRegistry;

    };
}