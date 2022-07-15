
#include "SR_ShaderState_DX12.h"

#if SR_ENABLE_DX12
#include "SR_PipelineStreamStructs_DX12.h"
#include "SR_RenderDevice_DX12.h"
#include "SR_BufferResource_DX12.h"
#include "SR_RootSignature_DX12.h"
#include <sstream>

SR_ShaderState_DX12::SR_ShaderState_DX12()
{

}

SR_ShaderState_DX12::~SR_ShaderState_DX12()
{

}

bool SR_ShaderState_DX12::Init(const SR_ShaderStateProperties& aProperties)
{
	for (uint32 i = 0; i < static_cast<uint32>(SR_ShaderType::COUNT); ++i)
		mShaderMetaDatas[i] = aProperties.mShaderMetaDatas[i];

#if SR_ENABLE_RAYTRACING
	if (aProperties.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Raytracing)].mSize)
		return InitAsRaytracingShader(aProperties);
	else 
#endif
	if (aProperties.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)].mSize)
		return InitAsComputeShader(aProperties);
#if SR_ENABLE_MESH_SHADERS
	else if (SR_RenderDevice::gInstance->GetSupportCaps().mEnableMeshShaders && aProperties.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Mesh)].mSize)
		return InitAsMeshShader(aProperties);
#endif
	else
		return InitDefault(aProperties);
}

ID3D12PipelineState* SR_ShaderState_DX12::GetD3D12PipelineState() const
{
	return mD3D12PipelineState.Get();
}

#if SR_ENABLE_RAYTRACING
const D3D12_DISPATCH_RAYS_DESC& SR_ShaderState_DX12::GetDispatchRaysDesc() const
{
	return mDispatchRaysDesc;
}

ID3D12StateObject* SR_ShaderState_DX12::GetD3D12StateObject() const
{
	return mD3D12StateObject.Get();
}
#endif

static D3D12_INPUT_ELEMENT_DESC CreateElementDesc(DXGI_FORMAT aFormat, uint32 aSemanticIndex, const char* aSemanticName, bool aIsInstanceData = false)
{
	D3D12_INPUT_ELEMENT_DESC element = {};
	element.Format = aFormat;
	element.InputSlot = aIsInstanceData ? 1 : 0;
	element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	element.InputSlotClass = aIsInstanceData ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	element.InstanceDataStepRate = aIsInstanceData ? 1 : 0;
	element.SemanticIndex = aSemanticIndex;
	element.SemanticName = aSemanticName;
	return element;
}

static const char* locGetVertexAttributeSemantic(const SR_VertexAttribute& aAttributeId)
{
	switch (aAttributeId)
	{
	case SR_VertexAttribute::Position: return "POSITION";
	case SR_VertexAttribute::Normal: return "NORMAL";
	case SR_VertexAttribute::Tangent: return "TANGENT";
	case SR_VertexAttribute::Bitangent: return "BITANGENT";
	case SR_VertexAttribute::UV: return "UV";
	case SR_VertexAttribute::Color: return "COLOR";
	case SR_VertexAttribute::BoneId: return "BONEID";
	case SR_VertexAttribute::BoneWeight: return "BONEWEIGHT";
	}

	SC_ASSERT(false, "Unknown attribute");
	return "<unknown>";
}

bool SR_ShaderState_DX12::InitDefault(const SR_ShaderStateProperties& aProperties)
{
	uint32 vsIndex = static_cast<uint32>(SR_ShaderType::Vertex);
	uint32 psIndex = static_cast<uint32>(SR_ShaderType::Pixel);

	SR_DefaultShaderPipelineStreamDesc streamDesc;
	streamDesc.mVS.mByteCode = { aProperties.mShaderByteCodes[vsIndex].mByteCode.get(), aProperties.mShaderByteCodes[vsIndex].mSize };
	streamDesc.mPS.mByteCode = { aProperties.mShaderByteCodes[psIndex].mByteCode.get(), aProperties.mShaderByteCodes[psIndex].mSize };

	SC_Array<D3D12_INPUT_ELEMENT_DESC> inputElements;
	
	for (const SR_VertexAttributeData& attribute : aProperties.mVertexLayout.mAttributes)
		inputElements.Add(CreateElementDesc(SR_D3D12ConvertFormat(attribute.mFormat), attribute.mAttributeIndex, locGetVertexAttributeSemantic(attribute.mAttributeId)));

	streamDesc.mVertexLayout.mDesc.NumElements = inputElements.Count();
	streamDesc.mVertexLayout.mDesc.pInputElementDescs = inputElements.GetBuffer();

	streamDesc.mRTVFormats.Init(aProperties.mRTVFormats);
	streamDesc.mDSVFormat.Init(aProperties.mRTVFormats.mDepthFormat);
	streamDesc.mRasterizerState.Init(aProperties.mRasterizerProperties);
	streamDesc.mBlendState.Init(aProperties.mBlendStateProperties);
	streamDesc.mDepthStencilState.Init(aProperties.mDepthStencilProperties);

	switch (aProperties.mPrimitiveTopology)
	{
	case SR_PrimitiveTopology::PointList:
		streamDesc.mPrimitiveTopology.mValue = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		break;
	case SR_PrimitiveTopology::LineList:
	case SR_PrimitiveTopology::LineStrip:
		streamDesc.mPrimitiveTopology.mValue = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		break;
	case SR_PrimitiveTopology::TriangleList:
	default:
		streamDesc.mPrimitiveTopology.mValue = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		break;
	}

	SR_RootSignature* rootSignature = (aProperties.mRootSignature) ? aProperties.mRootSignature.Get() : SR_RenderDevice::gInstance->GetRootSignature(SR_RootSignatureType::Graphics);
	SR_RootSignature_DX12* rootSignatureDX12 = static_cast<SR_RootSignature_DX12*>(rootSignature);
	streamDesc.mRootSignature.mPtr = rootSignatureDX12->GetD3D12RootSignature();
	mRootSignature = rootSignature;

	D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
	desc.pPipelineStateSubobjectStream = &streamDesc;
	desc.SizeInBytes = sizeof(streamDesc);

	HRESULT result = E_FAIL;
	result = SR_RenderDevice_DX12::gInstance->GetD3D12Device5()->CreatePipelineState(&desc, IID_PPV_ARGS(&mD3D12PipelineState));

	return VerifyHRESULT(result);
}

#if SR_ENABLE_MESH_SHADERS
bool SR_ShaderState_DX12::InitAsMeshShader(const SR_ShaderStateProperties& aProperties)
{
	uint32 asIndex = static_cast<uint32>(SR_ShaderType::Amplification);
	uint32 msIndex = static_cast<uint32>(SR_ShaderType::Mesh);
	uint32 psIndex = static_cast<uint32>(SR_ShaderType::Pixel);

	bool hasAmplificationShader = aProperties.mShaderByteCodes[asIndex].mSize;

	SR_RootSignature* rootSignature = (aProperties.mRootSignature) ? aProperties.mRootSignature.Get() : SR_RenderDevice::gInstance->GetRootSignature(SR_RootSignatureType::GraphicsMS);
	SR_RootSignature_DX12* rootSignatureDX12 = static_cast<SR_RootSignature_DX12*>(rootSignature);

	HRESULT result = E_FAIL;
	if (hasAmplificationShader)
	{
		SR_MeshShaderPipelineStreamDesc_WithAS streamDesc;
		streamDesc.mRootSignature.mPtr = rootSignatureDX12->GetD3D12RootSignature();

		streamDesc.mAS.mByteCode = { aProperties.mShaderByteCodes[asIndex].mByteCode.get(), aProperties.mShaderByteCodes[asIndex].mSize };
		streamDesc.mMS.mByteCode = { aProperties.mShaderByteCodes[msIndex].mByteCode.get(), aProperties.mShaderByteCodes[msIndex].mSize };
		streamDesc.mPS.mByteCode = { aProperties.mShaderByteCodes[psIndex].mByteCode.get(), aProperties.mShaderByteCodes[psIndex].mSize };

		streamDesc.mRTVFormats.Init(aProperties.mRTVFormats);
		streamDesc.mDSVFormat.Init(aProperties.mRTVFormats.mDepthFormat);
		streamDesc.mRasterizerState.Init(aProperties.mRasterizerProperties);
		streamDesc.mBlendState.Init(aProperties.mBlendStateProperties);
		streamDesc.mDepthStencilState.Init(aProperties.mDepthStencilProperties);
		
		D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
		desc.pPipelineStateSubobjectStream = &streamDesc;
		desc.SizeInBytes = sizeof(streamDesc);

		result = SR_RenderDevice_DX12::gInstance->GetD3D12Device5()->CreatePipelineState(&desc, IID_PPV_ARGS(&mD3D12PipelineState));
	}
	else
	{
		SR_MeshShaderPipelineStreamDesc streamDesc;
		streamDesc.mRootSignature.mPtr = rootSignatureDX12->GetD3D12RootSignature();

		streamDesc.mMS.mByteCode = { aProperties.mShaderByteCodes[msIndex].mByteCode.get(), aProperties.mShaderByteCodes[msIndex].mSize };
		streamDesc.mPS.mByteCode = { aProperties.mShaderByteCodes[psIndex].mByteCode.get(), aProperties.mShaderByteCodes[psIndex].mSize };

		streamDesc.mRTVFormats.Init(aProperties.mRTVFormats);
		streamDesc.mDSVFormat.Init(aProperties.mRTVFormats.mDepthFormat);
		streamDesc.mRasterizerState.Init(aProperties.mRasterizerProperties);
		streamDesc.mBlendState.Init(aProperties.mBlendStateProperties);
		streamDesc.mDepthStencilState.Init(aProperties.mDepthStencilProperties);

		D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
		desc.pPipelineStateSubobjectStream = &streamDesc;
		desc.SizeInBytes = sizeof(streamDesc);

		result = SR_RenderDevice_DX12::gInstance->GetD3D12Device5()->CreatePipelineState(&desc, IID_PPV_ARGS(&mD3D12PipelineState));
	}
	mRootSignature = rootSignature;
	mIsMeshShader = true;
	return VerifyHRESULT(result);
}
#endif //SR_ENABLE_MESH_SHADERS

bool SR_ShaderState_DX12::InitAsComputeShader(const SR_ShaderStateProperties& aProperties)
{
	uint32 csIndex = static_cast<uint32>(SR_ShaderType::Compute);
	SR_RootSignature* rootSignature = (aProperties.mRootSignature) ? aProperties.mRootSignature.Get() : SR_RenderDevice::gInstance->GetRootSignature(SR_RootSignatureType::Compute);
	SR_RootSignature_DX12* rootSignatureDX12 = static_cast<SR_RootSignature_DX12*>(rootSignature);

	SR_ComputeShaderPipelineStreamDesc streamDesc;
	streamDesc.mRootSignature.mPtr = rootSignatureDX12->GetD3D12RootSignature();
	streamDesc.mCS.mByteCode = { aProperties.mShaderByteCodes[csIndex].mByteCode.get(), aProperties.mShaderByteCodes[csIndex].mSize };

	D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
	desc.pPipelineStateSubobjectStream = &streamDesc;
	desc.SizeInBytes = sizeof(streamDesc);

	HRESULT result = SR_RenderDevice_DX12::gInstance->GetD3D12Device5()->CreatePipelineState(&desc, IID_PPV_ARGS(&mD3D12PipelineState));
	mRootSignature = rootSignature;
	mIsComputeShader = true;
	return VerifyHRESULT(result);
}

#if SR_ENABLE_RAYTRACING
bool SR_ShaderState_DX12::InitAsRaytracingShader(const SR_ShaderStateProperties& aProperties)
{
	SC_Array<D3D12_STATE_SUBOBJECT> subObjects;

	D3D12_DXIL_LIBRARY_DESC libDesc = {};
	libDesc.DXILLibrary = { aProperties.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Raytracing)].mByteCode.get(), (SIZE_T)aProperties.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Raytracing)].mSize };
	subObjects.Add({ D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY, &libDesc });

	SR_RootSignature* rootSignature = (aProperties.mRootSignature) ? aProperties.mRootSignature.Get() : SR_RenderDevice::gInstance->GetRootSignature(SR_RootSignatureType::Raytracing);
	SR_RootSignature_DX12* rootSignatureDX12 = static_cast<SR_RootSignature_DX12*>(rootSignature);
	D3D12_GLOBAL_ROOT_SIGNATURE globalRootSig = { rootSignatureDX12->GetD3D12RootSignature() };
	subObjects.Add({ D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE, &globalRootSig });

	D3D12_RAYTRACING_SHADER_CONFIG shaderConfig = {};
	shaderConfig.MaxAttributeSizeInBytes = D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES;
	shaderConfig.MaxPayloadSizeInBytes = 32;
	subObjects.Add({ D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG, &shaderConfig });

	D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig = {};
	pipelineConfig.MaxTraceRecursionDepth = 1;
	subObjects.Add({ D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG, &pipelineConfig });

	SC_Array<std::wstring> strings;
	SC_Array<D3D12_HIT_GROUP_DESC> hitGroups;
	hitGroups.Reserve(aProperties.mHitGroups.Count());
	for (uint32 i = 0; i < aProperties.mHitGroups.Count(); ++i)
	{
		std::wstringstream name;
		D3D12_HIT_GROUP_DESC hitGroupDesc = {};

		CONST SR_ShaderStateProperties::RTHitGroup& hitGroup = aProperties.mHitGroups[i];

		hitGroupDesc.Type = (hitGroup.myHasIntersection) ? D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE : D3D12_HIT_GROUP_TYPE_TRIANGLES;

		name << L"HitGroup" << i;
		hitGroupDesc.HitGroupExport = strings.Add(name.str()).c_str();
		name.str(std::wstring());

		if (hitGroup.myHasClosestHit)
		{
			name << L"HitGroup" << i << L"_ClosestHit";
			hitGroupDesc.ClosestHitShaderImport = strings.Add(name.str()).c_str();
			name.str(std::wstring());
		}
		if (hitGroup.myHasAnyHit)
		{
			name << L"HitGroup" << i << L"_AnyHit";
			hitGroupDesc.AnyHitShaderImport = strings.Add(name.str()).c_str();
			name.str(std::wstring());
		}
		if (hitGroup.myHasIntersection)
		{
			name << L"HitGroup" << i << L"_Intersection";
			hitGroupDesc.IntersectionShaderImport = strings.Add(name.str()).c_str();
			name.str(std::wstring());
		}

		hitGroups.Add(hitGroupDesc);
		subObjects.Add({ D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP, &hitGroups[i] });
	}

	D3D12_STATE_OBJECT_DESC desc = {};
	desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	desc.pSubobjects = subObjects.GetBuffer();
	desc.NumSubobjects = subObjects.Count();

	HRESULT result = SR_RenderDevice_DX12::gInstance->GetD3D12Device5()->CreateStateObject(&desc, IID_PPV_ARGS(&mD3D12StateObject));

	mRootSignature = rootSignature;
	mIsRaytracingShader = true;
	CreateRaytracingShaderTable(aProperties);
	return VerifyHRESULT(result);
}

void SR_ShaderState_DX12::CreateRaytracingShaderTable(const SR_ShaderStateProperties& aProperties)
{
	SR_ComPtr<ID3D12StateObjectProperties> stateObjectProps;
	mD3D12StateObject.As(&stateObjectProps);

	uint32 tableSize = SC_Align(uint32(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES), uint32(D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT));
	uint32 recordSize = SC_Align(uint32(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES), uint32(D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT));

	uint32 rayGenOffset = 0;
	uint32 missOffset = 1 * tableSize;
	uint32 hitGroupsOffset = 2 * tableSize;

	uint32 numHitGroups = aProperties.mHitGroups.Count();
	uint32 hitGroupsSize = SC_Align(numHitGroups * recordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

	SR_BufferResourceProperties bufferProps;
	bufferProps.mElementCount = 2 * tableSize + hitGroupsSize;
	bufferProps.mElementSize = 1;
	//bufferProps.mBindFlags = SR_BufferBindFlag_ConstantBuffer; // not really, but this gives sufficient alignment

	SC_Array<char> data;
	data.Respace(bufferProps.mElementCount);

	const void* main = stateObjectProps->GetShaderIdentifier(L"Main");
	SC_ASSERT(main);
	if (main)
		SC_Memcpy(data.GetBuffer() + rayGenOffset, main, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

	const void* miss = stateObjectProps->GetShaderIdentifier(L"Miss");
	if (miss)
		SC_Memcpy(data.GetBuffer() + missOffset, miss, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

	std::wstringstream name;
	for (uint32 i = 0; i != numHitGroups; ++i)
	{
		name << L"HitGroup" << i;
		const void* id = stateObjectProps->GetShaderIdentifier(name.str().c_str());
		SC_ASSERT(id);
		SC_Memcpy(data.GetBuffer() + hitGroupsOffset + (uint64)i * (uint64)recordSize, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		name.str(std::wstring());
	}

	mRaytracingShaderTable = SR_RenderDevice_DX12::gInstance->CreateBufferResource(bufferProps, *data/*, L"RT Shader Table"*/);
	SR_BufferResource_DX12* buffer = static_cast<SR_BufferResource_DX12*>(mRaytracingShaderTable.Get());

	D3D12_GPU_VIRTUAL_ADDRESS address = buffer->GetD3D12Resource()->GetGPUVirtualAddress();
	mDispatchRaysDesc = {};
	mDispatchRaysDesc.RayGenerationShaderRecord.StartAddress = address + rayGenOffset;
	mDispatchRaysDesc.RayGenerationShaderRecord.SizeInBytes = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	mDispatchRaysDesc.MissShaderTable.StartAddress = address + missOffset;
	mDispatchRaysDesc.MissShaderTable.SizeInBytes = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	mDispatchRaysDesc.MissShaderTable.StrideInBytes = 0;
	mDispatchRaysDesc.HitGroupTable.StartAddress = address + hitGroupsOffset;
	mDispatchRaysDesc.HitGroupTable.SizeInBytes = hitGroupsSize;
	mDispatchRaysDesc.HitGroupTable.StrideInBytes = numHitGroups > 1 ? recordSize : 0; // 0 stride when possible as a hint to the driver, recommended by Intel
}
#endif //SR_ENABLE_RAYTRACING
#endif //SR_ENABLE_DX12