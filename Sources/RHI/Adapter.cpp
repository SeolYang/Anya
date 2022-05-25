#include <PCH.h>
#include <RHI/Adapter.h>
#include <Core/Exceptions.h>

namespace sy
{
	Adapter::Adapter(const ComPtr<IDXGIAdapter1>& _adapter) :
		adapter(_adapter)
	{
		UINT idx = 0;
		ComPtr<IDXGIOutput> output;

		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		size_t returned = 0;
		char convertedDescBuffer[256] = { 0, };
		wcstombs_s(&returned, convertedDescBuffer, desc.Description, 255);
		description = convertedDescBuffer;

		vendorID = desc.VendorId;
		deviceID = desc.DeviceId;
		subSysID = desc.SubSysId;
		revision = desc.Revision;
		dedicatedVideoMemory = desc.DedicatedVideoMemory;
		dedicatedSystemMemory = desc.DedicatedSystemMemory;
		sharedSystemMemory = desc.SharedSystemMemory;

		while (adapter->EnumOutputs(idx++, &output) != DXGI_ERROR_NOT_FOUND)
		{
			ComPtr<IDXGIOutput6> output6;
			DXCall(output.As(&output6));

			DXGI_OUTPUT_DESC1 desc;
			DXCall(output6->GetDesc1(&desc));
			displays.emplace_back(desc);
		}
	}
}