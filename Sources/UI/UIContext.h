#pragma once
#include <PCH.h>

namespace sy
{
    class UIContext
    {
    public:
        UIContext()
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();

            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        }

        void BeginFrame()
        {
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
        }

        void Render()
        {
            ImGui::Render();
        }

        ~UIContext()
        {
            ImGui::DestroyContext();
        }

    };
}