#pragma once
#include <PCH.h>

namespace sy
{
    /**
     * @todo Organize UI Framework
     */
    class Application;
    class ApplicationMenuBar
    {
    public:
        ApplicationMenuBar(Application& application);

        void OnGUI();

    private:
        Application& application;

    };
}