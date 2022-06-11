#pragma once
#include <PCH.h>

namespace sy
{
    class UIContext
    {
    public:
        UIContext(const bool bEnableDocking = false) :
            bEnableDocking(bEnableDocking)
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();
            //ImGui::Spectrum::LoadFont();

            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            if (bEnableDocking)
            {
                io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
                io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            }
        }

        void BeginFrame()
        {
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            if (bEnableDocking)
            {
                ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
            }
        }

        void EndFrame()
        {
            ImGui::EndFrame();


            if (bEnableDocking)
            {
                const ImGuiIO& io = ImGui::GetIO(); (void)io;
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                }
            }
        }

        void Render()
        {
            ImGui::Render();
        }

        ~UIContext()
        {
            ImGui::DestroyContext();
        }

    private:
        bool bEnableDocking;

    };
}