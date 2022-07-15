#pragma once
#include "RenderCore/Interface/SR_RenderStates.h"

#if SR_ENABLE_DX12

struct alignas(void*) SR_PipelineStreamRasterizerDesc
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER;
	D3D12_RASTERIZER_DESC mDesc;

public:
	void Init(const SR_RasterizerStateProperties& aProperties)
	{
		mDesc.FillMode = (aProperties.mWireframe) ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
		mDesc.CullMode = SR_D3D12ConvertCullMode(aProperties.mCullMode);
		mDesc.FrontCounterClockwise = aProperties.mCounterClockWise;
		mDesc.DepthBias = aProperties.mDepthBias;
		mDesc.DepthBiasClamp = aProperties.mDepthBiasClamp;
		mDesc.SlopeScaledDepthBias = aProperties.mSlopedScaleDepthBias;
		mDesc.DepthClipEnable = aProperties.mEnableDepthClip;
		mDesc.MultisampleEnable = false;
		mDesc.AntialiasedLineEnable = false;
		mDesc.ForcedSampleCount = 0;
		mDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}
};

struct alignas(void*) SR_PipelineStreamBlendDesc
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND;
	D3D12_BLEND_DESC mDesc;

public:
	void Init(const SR_BlendStateProperties& aProperties)
	{
		mDesc.AlphaToCoverageEnable = aProperties.mAlphaToCoverage;
		mDesc.IndependentBlendEnable = (aProperties.mNumRenderTargets > 0);
		for (uint32 RtIndex = 0; RtIndex < SR_MAX_RENDER_TARGETS; ++RtIndex)
		{
			D3D12_RENDER_TARGET_BLEND_DESC& rtBlendDescDst = mDesc.RenderTarget[RtIndex];
			const SR_RenderTargetBlendProperties& rtBlendProps = aProperties.mRenderTagetBlendProperties[RtIndex];
			rtBlendDescDst.BlendEnable = rtBlendProps.mEnableBlend;
			rtBlendDescDst.BlendOp = SR_D3D12ConvertBlendFunc(rtBlendProps.mBlendFunc);
			rtBlendDescDst.BlendOpAlpha = SR_D3D12ConvertBlendFunc(rtBlendProps.mBlendFuncAlpha);
			rtBlendDescDst.SrcBlend = SR_D3D12ConvertBlendMode(rtBlendProps.mSrcBlend);
			rtBlendDescDst.SrcBlendAlpha = SR_D3D12ConvertBlendMode(rtBlendProps.mSrcBlendAlpha);
			rtBlendDescDst.DestBlend = SR_D3D12ConvertBlendMode(rtBlendProps.mDstBlend);
			rtBlendDescDst.DestBlendAlpha = SR_D3D12ConvertBlendMode(rtBlendProps.mDstBlendAlpha);
			rtBlendDescDst.RenderTargetWriteMask = SR_D3D12ConvertColorWriteMask(rtBlendProps.mWriteMask);
			rtBlendDescDst.LogicOp = D3D12_LOGIC_OP_NOOP;
			rtBlendDescDst.LogicOpEnable = false;

			if (!(rtBlendDescDst.RenderTargetWriteMask & D3D12_COLOR_WRITE_ENABLE_ALPHA))
			{
				rtBlendDescDst.SrcBlendAlpha = D3D12_BLEND_ZERO;
				rtBlendDescDst.DestBlendAlpha = D3D12_BLEND_ONE;
			}
		}
	}
};

struct alignas(void*) SR_PipelineStreamDepthStencilDesc
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL;
	D3D12_DEPTH_STENCIL_DESC mDesc;

public:
	void Init(const SR_DepthStencilStateProperties& aProperties)
	{
		mDesc.DepthEnable = (aProperties.mDepthComparisonFunc != SR_ComparisonFunc::Always || aProperties.mWriteDepth) ? true : false;
		mDesc.DepthWriteMask = (aProperties.mWriteDepth) ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
		mDesc.DepthFunc = SR_D3D12ConvertComparisonFunc(aProperties.mDepthComparisonFunc);
		mDesc.StencilEnable = aProperties.mEnableStencil;
		mDesc.StencilReadMask = aProperties.mStencilReadMask;
		mDesc.StencilWriteMask = aProperties.mStencilWriteMask;
		mDesc.FrontFace.StencilFailOp = SR_D3D12ConvertStencilOperator(aProperties.mFrontFace.mFail);
		mDesc.FrontFace.StencilDepthFailOp = SR_D3D12ConvertStencilOperator(aProperties.mFrontFace.mDepthFail);
		mDesc.FrontFace.StencilPassOp = SR_D3D12ConvertStencilOperator(aProperties.mFrontFace.mPass);
		mDesc.FrontFace.StencilFunc = SR_D3D12ConvertComparisonFunc(aProperties.mFrontFace.mComparisonFunc);
		mDesc.BackFace.StencilFailOp = SR_D3D12ConvertStencilOperator(aProperties.mBackFace.mFail);
		mDesc.BackFace.StencilDepthFailOp = SR_D3D12ConvertStencilOperator(aProperties.mBackFace.mDepthFail);
		mDesc.BackFace.StencilPassOp = SR_D3D12ConvertStencilOperator(aProperties.mBackFace.mPass);
		mDesc.BackFace.StencilFunc = SR_D3D12ConvertComparisonFunc(aProperties.mBackFace.mComparisonFunc);
	}
};

struct alignas(void*) SR_PipelineStreamRenderTargetFormatsDesc
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS;
	D3D12_RT_FORMAT_ARRAY mDesc;

public:
	SR_PipelineStreamRenderTargetFormatsDesc()
	{
		for (uint32 i = 0; i < SR_MAX_RENDER_TARGETS; ++i)
		{
			mDesc.RTFormats[i] = DXGI_FORMAT_UNKNOWN;
		}
	}

	void Init(const SR_RenderTargetFormats& aRTVFormats)
	{
		mDesc.NumRenderTargets = aRTVFormats.mNumColorFormats;
		for (uint32 i = 0; i < aRTVFormats.mNumColorFormats; ++i)
			mDesc.RTFormats[i] = SR_D3D12ConvertFormat(aRTVFormats.mColorFormats[i]);
	}
};

struct alignas(void*) SR_PipelineStreamDepthStencilFormat
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT;
	DXGI_FORMAT mFormat;

public:
	void Init(const SR_Format& aFormat)
	{
		mFormat = SR_D3D12ConvertFormat(aFormat);
	}
};

struct alignas(void*) SR_PipelineStreamRootSignature
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE;
public:
	ID3D12RootSignature* mPtr;
};

struct alignas(void*) SR_PipelineStreamVertexLayout
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT;
public:
	D3D12_INPUT_LAYOUT_DESC mDesc;
};

struct alignas(void*) SR_PipelineStreamPimitiveTopology
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY;
public:
	D3D12_PRIMITIVE_TOPOLOGY_TYPE mValue;
};

struct alignas(void*) SR_PipelineStreamVertexShader
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS;
public:
	D3D12_SHADER_BYTECODE mByteCode;
};

#if SR_ENABLE_MESH_SHADERS
struct alignas(void*) SR_PipelineStreamAmplificationShader
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS;
public:
	D3D12_SHADER_BYTECODE mByteCode;
};

struct alignas(void*) SR_PipelineStreamMeshShader
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS;
public:
	D3D12_SHADER_BYTECODE mByteCode;
};
#endif

struct alignas(void*) SR_PipelineStreamPixelShader
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS;
public:
	D3D12_SHADER_BYTECODE mByteCode;
};

struct alignas(void*) SR_PipelineStreamComputeShader
{
private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE mType = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS;
public:
	D3D12_SHADER_BYTECODE mByteCode;
};

struct SR_DefaultShaderPipelineStreamDesc
{
	SR_PipelineStreamRasterizerDesc mRasterizerState;
	SR_PipelineStreamBlendDesc mBlendState;
	SR_PipelineStreamDepthStencilDesc mDepthStencilState;
	SR_PipelineStreamRenderTargetFormatsDesc mRTVFormats;
	SR_PipelineStreamDepthStencilFormat mDSVFormat;
	SR_PipelineStreamRootSignature mRootSignature;
	SR_PipelineStreamVertexLayout mVertexLayout;
	SR_PipelineStreamPimitiveTopology mPrimitiveTopology;
	SR_PipelineStreamVertexShader mVS;
	SR_PipelineStreamPixelShader mPS;
};

#if SR_ENABLE_MESH_SHADERS
struct SR_MeshShaderPipelineStreamDesc
{
	SR_PipelineStreamRasterizerDesc mRasterizerState;
	SR_PipelineStreamBlendDesc mBlendState;
	SR_PipelineStreamDepthStencilDesc mDepthStencilState;
	SR_PipelineStreamRenderTargetFormatsDesc mRTVFormats;
	SR_PipelineStreamDepthStencilFormat mDSVFormat;
	SR_PipelineStreamRootSignature mRootSignature;
	SR_PipelineStreamMeshShader mMS;
	SR_PipelineStreamPixelShader mPS;
};

struct SR_MeshShaderPipelineStreamDesc_WithAS : public SR_MeshShaderPipelineStreamDesc
{
	SR_PipelineStreamAmplificationShader mAS;
};
#endif

struct SR_ComputeShaderPipelineStreamDesc
{
	SR_PipelineStreamRootSignature mRootSignature;
	SR_PipelineStreamComputeShader mCS;
};

#endif