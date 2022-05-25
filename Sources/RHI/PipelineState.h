#pragma once
#include <PCH.h>

namespace sy
{
	class RootSignature;
	class Device;
	class PipelineState
	{
	public:
		PipelineState(const class Device& device);

		inline auto D3DPipelineState() const { return pipelineState.Get(); }
		void SetRootSignature(const RootSignature& signature) { rootSignature = &signature; }

		virtual void Build() = 0;

	private:
		ComPtr<ID3D12PipelineState> pipelineState;
		const class Device* device;
		const class RootSignature* rootSignature;

	};
}