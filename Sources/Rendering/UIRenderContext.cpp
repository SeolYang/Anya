#include <PCH.h>
#include <Rendering/UIRenderContext.h>
#include <Rendering/RenderContext.h>
#include <Rendering/SwapChain.h>
#include <RHI/Device.h>

#include "RHI/CommandList.h"

namespace sy
{
    UIRenderContext::UIRenderContext(HWND windowHandle, RHI::Device& device, const SwapChain& swapChain) :
        srDescriptorHeap(std::make_unique<RHI::SRDescriptorHeap>(device, 0, 1, 0))
    {

        auto* fontDescriptorHeap = srDescriptorHeap->GetD3DDescriptorHeap();
        ImGui_ImplWin32_Init(windowHandle);
        ImGui_ImplDX12_Init(device.GetD3DDevice(), 
            RenderContext::SimultaneousFrames, 
            swapChain.GetDesc().Format,
            fontDescriptorHeap, 
            fontDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            fontDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    }

    UIRenderContext::~UIRenderContext()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
    }

    void UIRenderContext::Render(const RHI::DirectCommandList& cmdList) const
    {
        std::vector<ConstRef<RHI::DescriptorHeap>> descriptorHeaps;
        descriptorHeaps.emplace_back(*srDescriptorHeap);
        cmdList.SetDescriptorHeaps(descriptorHeaps);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.GetD3DCommandList());
    }
}
