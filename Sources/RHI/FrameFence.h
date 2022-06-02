#pragma once
#include <PCH.h>
#include <RHI/Fence.h>

namespace sy::RHI
{
    class FrameFence : public Fence
    {
    public:
        FrameFence(const Device& device, size_t allowedNumOfSimultaneousFrames);

        void WaitForSimultaneousFramesCompletion();

    private:
        const size_t allowedNumOfSimultaneousFrames;

    };
}
