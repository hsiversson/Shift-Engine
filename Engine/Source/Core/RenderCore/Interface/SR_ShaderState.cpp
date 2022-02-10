#include "SR_ShaderState.h"

SR_ShaderState::SR_ShaderState()
	: mRootSignature(nullptr)
	, mIsMeshShader(false)
	, mIsComputeShader(false)
	, mIsRaytracingShader(false)
{

}

SR_ShaderState::~SR_ShaderState()
{

}
