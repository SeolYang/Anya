#include <PCH.h>
#include <RHI/AdapterPatcher.h>
#include <Core/CommandLineParser.h>
#include <Core/Exceptions.h>

namespace sy
{
	AdapterPatcher::AdapterPatcher(const CommandLineParser& commandLineParser)
	{
		UINT dxgiFactoryFlags = 0;

		if (commandLineParser.ShouldEnableDebugLayer())
		{
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		DXCall(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));
		ComPtr<IDXGIAdapter1> adapter;

		UINT idx = 0;
		while (dxgiFactory->EnumAdapters1(idx++, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue; // Should i support warp too?
			}
			else
			{
				hardwareAdapters.emplace_back(adapter);
			}
		}
	}
}