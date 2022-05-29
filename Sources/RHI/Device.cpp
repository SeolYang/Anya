#include <PCH.h>
#include <RHI/Device.h>
#include <RHI/Adapter.h>
#include <RHI/DebugLayer.h>
#include <Core/Exceptions.h>

namespace sy
{
	Device::Device(const Adapter& adapter)
	{
		/** https://docs.microsoft.com/en-us/windows/win32/direct3d12/hardware-feature-levels */
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_2;
		DXCall(D3D12CreateDevice(adapter.D3DAdapter(), featureLevel, IID_PPV_ARGS(&device)));

#if (defined(DEBUG) || defined(_DEBUG)) && !FORCE_DISABLE_DEBUG_LAYER
		/* https://www.3dgep.com/learning-directx-12-1/ */
		if (SUCCEEDED(device.As(&infoQueue)))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			D3D12_MESSAGE_SEVERITY denySeverities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			D3D12_MESSAGE_ID denyMessages[] =
			{
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
			};

			D3D12_INFO_QUEUE_FILTER infoQueueFilter{};
			infoQueueFilter.DenyList.NumSeverities = _countof(denySeverities);
			infoQueueFilter.DenyList.NumIDs = _countof(denyMessages);
			infoQueueFilter.DenyList.pSeverityList = denySeverities;
			infoQueueFilter.DenyList.pIDList = denyMessages;
			DXCall(infoQueue->PushStorageFilter(&infoQueueFilter));
		}
#endif

		SetDebugName(TEXT("Device"));
	}

	void Device::SetDebugName(const std::wstring_view debugName)
	{
		RHIObject::SetDebugName(debugName);
		if (device != nullptr)
		{
			DXCall(device->SetName(debugName.data()));
		}
	}
}