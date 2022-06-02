#include <PCH.h>
#include <RHI/FrameFence.h>

namespace sy::RHI
{
    FrameFence::FrameFence(const Device& device, size_t allowedNumOfSimultaneousFrames) :
        Fence(device),
        allowedNumOfSimultaneousFrames(allowedNumOfSimultaneousFrames)
    {
        SetDebugName(TEXT("Frame Fence"));
    }

    void FrameFence::WaitForSimultaneousFramesCompletion()
    {
        const auto completedFenceValue = this->CompletedValue();
        const auto numOfFramesInFlight = this->Value() - completedFenceValue;

        if (allowedNumOfSimultaneousFrames <= numOfFramesInFlight)
        {
            const HANDLE fenceEventHandle = CreateEventHandle();

            /** 한개 프레임이라도 여유가 나면 바로 다음 프레임을 작업할 수 있도록. */
            const auto valueToWaitFor = this->Value() - (allowedNumOfSimultaneousFrames - 1);
            SetEventOnCompletion(valueToWaitFor, fenceEventHandle);

            ::WaitForSingleObject(fenceEventHandle, INFINITE);
            ::CloseHandle(fenceEventHandle);
        }
    }
}