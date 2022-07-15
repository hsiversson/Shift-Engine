#include "SR_RenderContext.h"

SR_RenderContext::SR_RenderContext(const SR_RenderContextType& aContextType)
	: mContextType(aContextType)
{
}

SR_RenderContext::~SR_RenderContext()
{
}

void SR_RenderContext::Dispatch(const SC_IntVector3& aThreadGroups)
{
	Dispatch(aThreadGroups.x, aThreadGroups.y, aThreadGroups.z);
}

void SR_RenderContext::Dispatch(SR_ShaderState* aShader, uint32 aThreadGroupX, uint32 aThreadGroupY, uint32 aThreadGroupZ)
{
	const SC_IntVector& numThreads = aShader->GetMetaData(SR_ShaderType::Compute).mNumThreads;
	SC_IntVector actualThreadGroups;
	actualThreadGroups.x = SC_Max((aThreadGroupX + numThreads.x - 1) / numThreads.x, 1);
	actualThreadGroups.y = SC_Max((aThreadGroupY + numThreads.y - 1) / numThreads.y, 1);
	actualThreadGroups.z = SC_Max((aThreadGroupZ + numThreads.z - 1) / numThreads.z, 1);
	SetShaderState(aShader);
	Dispatch(actualThreadGroups);
}

void SR_RenderContext::Dispatch(SR_ShaderState* aShader, const SC_IntVector3& aThreadGroups)
{
	const SC_IntVector& numThreads = aShader->GetMetaData(SR_ShaderType::Compute).mNumThreads;
	SC_IntVector actualThreadGroups;
	actualThreadGroups.x = SC_Max((aThreadGroups.x + numThreads.x - 1) / numThreads.x, 1);
	actualThreadGroups.y = SC_Max((aThreadGroups.y + numThreads.y - 1) / numThreads.y, 1);
	actualThreadGroups.z = SC_Max((aThreadGroups.z + numThreads.z - 1) / numThreads.z, 1);
	SetShaderState(aShader);
	Dispatch(actualThreadGroups);
}
