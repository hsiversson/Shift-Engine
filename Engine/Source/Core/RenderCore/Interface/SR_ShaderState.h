#pragma once
#include "SR_RenderStates.h"
#include "SR_VertexLayout.h"
#include "RenderCore/ShaderCompiler/SR_ShaderCompileStructs.h"

class SR_RootSignature;

struct SR_ShaderMetaData
{
	SC_IntVector mNumThreads;
};

struct SR_ShaderStateProperties
{
	SR_ShaderStateProperties() : mPrimitiveTopology(SR_PrimitiveTopology::TriangleList) {}

	SR_ShaderMetaData mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::COUNT)];
	SR_ShaderByteCode mShaderByteCodes[static_cast<uint32>(SR_ShaderType::COUNT)];

	SR_VertexLayout mVertexLayout;
	SC_Ref<SR_RootSignature> mRootSignature;

	SR_RasterizerStateProperties mRasterizerProperties;
	SR_BlendStateProperties mBlendStateProperties;
	SR_DepthStencilStateProperties mDepthStencilProperties;
	SR_RenderTargetFormats mRTVFormats;
	SR_PrimitiveTopology mPrimitiveTopology;
	
	struct RTHitGroup
	{
		RTHitGroup() : myHasClosestHit(false), myHasAnyHit(false), myHasIntersection(false) {}
		bool myHasClosestHit : 1;
		bool myHasAnyHit : 1;
		bool myHasIntersection : 1;
	};
	SC_Array<RTHitGroup> mHitGroups;

};

class SR_ShaderState
{
public:
	virtual ~SR_ShaderState();

	const bool IsMeshShader() const { return mIsMeshShader; }
	const bool IsComputeShader() const { return mIsComputeShader; }
	const bool IsRaytracingShader() const { return mIsRaytracingShader; }

	SR_RootSignature* GetRootSignature() const { return mRootSignature; }

	const SR_ShaderMetaData& GetMetaData(const SR_ShaderType& aShaderType) const { return mShaderMetaDatas[static_cast<uint32>(aShaderType)]; }

protected:
	SR_ShaderState();

	SR_ShaderMetaData mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::COUNT)];
	SR_RootSignature* mRootSignature;
	bool mIsMeshShader : 1;
	bool mIsComputeShader : 1;
	bool mIsRaytracingShader : 1;
};

