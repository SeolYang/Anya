#pragma once
#include <PCH.h>
#include <Core/CommandLineParser.h>
#include <Core/ECS.h>

namespace anya
{
    class Application
    {
    public:
        Application(std::wstring_view title, int32 argc, wchar_t** argv);
        virtual ~Application();

        Application(const Application&) = delete;
        Application(Application&&) = delete;

        int32 Execute();

        std::wstring_view Title() const { return title; }

    private:
        std::wstring title;
        CommandLineParser cmdLineParser;
        std::unique_ptr<ComponentPoolRegistry> componentPoolRegistry;

    };
}