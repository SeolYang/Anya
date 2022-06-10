#include <PCH.h>
#include <UI/ApplicationMenuBar.h>
#include <Core/Application.h>

namespace sy
{
    ApplicationMenuBar::ApplicationMenuBar(Application& application) :
        application(application)
    {
    }

    void ApplicationMenuBar::OnGUI()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 7.f));
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Close"))
            {
                application.Close();
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
        ImGui::PopStyleVar();
    }
}