#pragma once
#include <PCH.h>
#include <RHI/Adapter.h>

namespace sy
{
	class CommandLineParser;
}

namespace sy::RHI
{
	class AdapterPatcher
	{
	public:
		AdapterPatcher(const CommandLineParser& commandLineParser);

		Adapter& operator[](size_t idx) { return hardwareAdapters[idx]; }
		const std::vector<Adapter>& HardwareAdapters() const { return hardwareAdapters; }

	private:
		ComPtr<IDXGIFactory7> dxgiFactory;
		std::vector<Adapter> hardwareAdapters;

	};
}