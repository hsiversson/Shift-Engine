#include "SR_RootSignature_DX12.h"

#if ENABLE_DX12
#include "SR_RenderDevice_DX12.h"

static D3D12_ROOT_SIGNATURE_FLAGS GetRootSignatureFlags(uint32 aFlags)
{
	D3D12_ROOT_SIGNATURE_FLAGS result = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	if (aFlags & SR_RootSignatureFlag_AllowInputAssembler)
		result |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	if (aFlags & SR_RootSignatureFlag_DenyVertexShaderAccess)
		result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	if (aFlags & SR_RootSignatureFlag_DenyHullShaderAccess)
		result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	if (aFlags & SR_RootSignatureFlag_DenyDomainShaderAccess)
		result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	if (aFlags & SR_RootSignatureFlag_DenyGeometryShaderAccess)
		result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	if (aFlags & SR_RootSignatureFlag_DenyPixelShaderAccess)
		result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	if (aFlags & SR_RootSignatureFlag_DenyAmplificationShaderAccess)
		result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
	if (aFlags & SR_RootSignatureFlag_DenyMeshShaderAccess)
		result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
	if (aFlags & SR_RootSignatureFlag_DescriptorHeapDirectAccess)
		result |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
	if (aFlags & SR_RootSignatureFlag_SamplerHeapDirectAccess)
		result |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

	return result;
}

static D3D12_SHADER_VISIBILITY GetShaderVisibility(const SR_RootParamVisibility& aVisibility)
{
	switch (aVisibility)
	{
	case SR_RootParamVisibility::All:
		return D3D12_SHADER_VISIBILITY_ALL;
	case SR_RootParamVisibility::Vertex:
		return D3D12_SHADER_VISIBILITY_VERTEX;
	case SR_RootParamVisibility::Hull:
		return D3D12_SHADER_VISIBILITY_HULL;
	case SR_RootParamVisibility::Domain:
		return D3D12_SHADER_VISIBILITY_DOMAIN;
	case SR_RootParamVisibility::Geometry:
		return D3D12_SHADER_VISIBILITY_GEOMETRY;
	case SR_RootParamVisibility::Pixel:
		return D3D12_SHADER_VISIBILITY_PIXEL;
	case SR_RootParamVisibility::Amplification:
		return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
	case SR_RootParamVisibility::Mesh:
		return D3D12_SHADER_VISIBILITY_MESH;
	}

	assert(false && "Incorrect visibility.");
	return D3D12_SHADER_VISIBILITY_ALL;
}

SR_RootSignature_DX12::SR_RootSignature_DX12(const SR_RootSignatureProperties& aProperties)
	: SR_RootSignature(aProperties)
{

}

SR_RootSignature_DX12::~SR_RootSignature_DX12()
{

}

bool SR_RootSignature_DX12::Init()
{
	SC_Array<D3D12_ROOT_PARAMETER1> rootParams;
	SC_Array<SC_Array<D3D12_DESCRIPTOR_RANGE1>> descriptorRanges;

	for (const SR_RootParam& rootParam : mProperties.mRootParams)
	{
		D3D12_ROOT_PARAMETER1& param = rootParams.Add();
		param.ShaderVisibility = GetShaderVisibility(rootParam.mVisibility);

		switch (rootParam.mType)
		{
		case SR_RootParamType::Table:
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			break;
		case SR_RootParamType::CBV:
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			break;
		case SR_RootParamType::SRV:
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			break;
		case SR_RootParamType::UAV:
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
			break;
		}

		if (rootParam.mType == SR_RootParamType::Table)
		{
			SC_Array<D3D12_DESCRIPTOR_RANGE1>& ranges = descriptorRanges.Add();
			for (const SR_RootTableRange& descriptorRange : rootParam.mDescriptorTable.mDescriptorRanges)
			{
				D3D12_DESCRIPTOR_RANGE1& range = ranges.Add();
				range.BaseShaderRegister = descriptorRange.mStartRegister;
				range.NumDescriptors = descriptorRange.mNumDescriptors;
				range.OffsetInDescriptorsFromTableStart = 0;
				range.RegisterSpace = descriptorRange.mRegisterSpace;
				range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;

				switch (descriptorRange.mDescriptorType)
				{
				case SR_DescriptorType::CBV:
					range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
					break;
				case SR_DescriptorType::SRV:
					range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
					break;
				case SR_DescriptorType::UAV:
					range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
					break;
				case SR_DescriptorType::Sampler:
					range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
					break;
				}
			}

			param.DescriptorTable.NumDescriptorRanges = ranges.Count();
			param.DescriptorTable.pDescriptorRanges = ranges.GetBuffer();
		}
		else
		{
			param.Descriptor.RegisterSpace = rootParam.mDescriptor.mRegisterSpace;
			param.Descriptor.ShaderRegister = rootParam.mDescriptor.mRegisterIndex;
			param.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE;
		}
	}

	SC_Array<D3D12_STATIC_SAMPLER_DESC> staticSamplers;
	uint32 currentRegisterIndex = 0;
	for (const SR_StaticSamplerProperties& staticSamplerProps : mProperties.mStaticSamplers)
	{
		static const SC_Vector4 transparentBlack(0);
		static const SC_Vector4 opaqueBlack(0, 0, 0, 1);
		static const SC_Vector4 opaqueWhite(1);

		const SR_SamplerProperties& sampler = staticSamplerProps.mProperties;
		D3D12_STATIC_SAMPLER_DESC& staticSampler = staticSamplers.Add();
		staticSampler.Filter = SR_D3D12ConvertFilter(sampler.mMinFilter, sampler.mMagFilter, sampler.mMipFilter, sampler.mComparison, sampler.mMaxAnisotropy);
		staticSampler.MaxAnisotropy = 16;
		staticSampler.AddressU = SR_D3D12ConvertWrapMode(sampler.mWrapX);
		staticSampler.AddressV = SR_D3D12ConvertWrapMode(sampler.mWrapY);
		staticSampler.AddressW = SR_D3D12ConvertWrapMode(sampler.mWrapZ);
		staticSampler.ComparisonFunc = SR_D3D12ConvertComparisonFunc(sampler.mComparison);
		staticSampler.MipLODBias = sampler.mLODBias;
		staticSampler.MinLOD = 0;
		staticSampler.MaxLOD = SC_FLT_MAX;
		staticSampler.RegisterSpace = 1;
		staticSampler.ShaderRegister = currentRegisterIndex++;
		staticSampler.ShaderVisibility = GetShaderVisibility(staticSamplerProps.mVisibility);

		if (sampler.mBorderColor == transparentBlack)
			staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		else if (sampler.mBorderColor == opaqueBlack)
			staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		else if (sampler.mBorderColor == opaqueWhite)
			staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		else
			assert(!"Unsupported Border Color");
	}

	D3D12_ROOT_SIGNATURE_DESC1 rootSigDesc = {};
	rootSigDesc.Flags = GetRootSignatureFlags(static_cast<SR_RootSignatureFlag>(mProperties.mFlags));
	rootSigDesc.NumParameters = rootParams.Count();
	rootSigDesc.pParameters = rootParams.GetBuffer();
	rootSigDesc.NumStaticSamplers = staticSamplers.Count();
	rootSigDesc.pStaticSamplers = staticSamplers.GetBuffer();

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc = {};
	desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
	desc.Desc_1_1 = rootSigDesc;

	SR_ComPtr<ID3DBlob> signatureBlob;
	SR_ComPtr<ID3DBlob> signatureError;
	HRESULT hr = D3D12SerializeVersionedRootSignature(&desc, &signatureBlob, &signatureError);
	if (!VerifyHRESULT(hr))
	{
		//if (signatureError)
		//{
		//	LOG_ERROR("{}", (char*)signatureError->GetBufferPointer());
		//}
		return false;
	}

	hr = SR_RenderDevice_DX12::gD3D12Instance->GetD3D12Device()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&mD3D12RootSignature));
	if (!VerifyHRESULT(hr))
		return false;

	if (mProperties.mDebugName)
	{
		hr = mD3D12RootSignature->SetName(SC_UTF8ToUTF16(mProperties.mDebugName).c_str());
		if (!VerifyHRESULT(hr))
			return false;
	}
	return true;
}

ID3D12RootSignature* SR_RootSignature_DX12::GetD3D12RootSignature() const
{
	return mD3D12RootSignature.Get();
}

#endif