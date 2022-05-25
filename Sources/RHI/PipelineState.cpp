#include <PCH.h>
#include <RHI/PipelineState.h>
#include <RHI/Device.h>
#include <RHI/RootSignature.h>

namespace sy
{
	PipelineState::PipelineState(const Device& device) :
		device(&device)
	{
	}
}