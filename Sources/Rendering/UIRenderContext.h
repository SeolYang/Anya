#pragma once
#include <PCH.h>
#include <RHI/DescriptorHeap.h>

namespace sy
{
    namespace RHI
    {
        class Device;
        class DirectCommandList;
    }

    class SwapChain;
    class UIRenderContext final
    {
    public:
        UIRenderContext(HWND windowHandle, RHI::Device& device, const SwapChain& swapChain);
        ~UIRenderContext();

        UIRenderContext(const UIRenderContext&) = delete;
        UIRenderContext(UIRenderContext&&) noexcept = delete;
        UIRenderContext& operator=(const UIRenderContext&) = delete;
        UIRenderContext& operator=(UIRenderContext&&) noexcept = delete;

        void Render(const RHI::DirectCommandList& cmdList) const;

    private:
        std::unique_ptr<RHI::SRDescriptorHeap> srDescriptorHeap;

    };
}