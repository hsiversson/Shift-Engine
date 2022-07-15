#include "SR_CommandList_DX12.h"

#if ENABLE_DX12
#include "SR_RenderDevice_DX12.h"
#include "SR_RenderTarget_DX12.h"
#include "SR_DepthStencil_DX12.h"
#include "SR_ShaderState_DX12.h"
#include "SR_Buffer_DX12.h"
#include "SR_BufferResource_DX12.h"
#include "SR_DescriptorHeap_DX12.h"
#include "SR_RootSignature_DX12.h"
#include "SR_TextureResource_DX12.h"
#include "RenderCore/Interface/SR_RaytracingStructs.h"

SR_CommandList_DX12::SR_CommandList_DX12(const SR_CommandListType& aType)
	: SR_CommandList(aType)
{

}

SR_CommandList_DX12::~SR_CommandList_DX12()
{

}

bool SR_CommandList_DX12::Init(const char* aDebugName)
{
	D3D12_COMMAND_LIST_TYPE cmdListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	switch (mType)
	{
	case SR_CommandListType::Compute:
		cmdListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	case SR_CommandListType::Copy:
		cmdListType = D3D12_COMMAND_LIST_TYPE_COPY;
		break;
	default:
		break;
	}

	HRESULT result = SR_RenderDevice_DX12::gInstance->GetD3D12Device()->CreateCommandAllocator(cmdListType, IID_PPV_ARGS(&mD3D12CommandAllocator));
	if (!VerifyHRESULT(result))
	{
		SC_ASSERT(false);
		return false;
	}
	
	result = SR_RenderDevice_DX12::gInstance->GetD3D12Device()->CreateCommandList(0, cmdListType, mD3D12CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mD3D12CommandList));
	if (!VerifyHRESULT(result))
	{
		SC_ASSERT(false);
		return false;
	}

	if (aDebugName)
	{
		mD3D12CommandList->SetName(SC_UTF8ToUTF16(aDebugName).c_str());
	}

	mD3D12CommandList.As(&mD3D12CommandList6);

	mD3D12CommandList->Close();
    return true;
}

void SR_CommandList_DX12::Begin()
{
	mD3D12CommandAllocator->Reset();
	mD3D12CommandList->Reset(mD3D12CommandAllocator.Get(), nullptr);
	mStateCache.Clear();

	SR_CommandList::Begin();
}

void SR_CommandList_DX12::End()
{
	mD3D12CommandList->Close();
}

void SR_CommandList_DX12::BeginEvent(const char* aName)
{
#if ENABLE_PIX
	PIXBeginEvent(mD3D12CommandList.Get(), 0, "%s", aName);
#else
	(void)aName;
#endif
}

void SR_CommandList_DX12::EndEvent()
{
#if ENABLE_PIX
	PIXEndEvent(mD3D12CommandList.Get());
#endif
}

void SR_CommandList_DX12::Dispatch(uint32 aGroupCountX, uint32 aGroupCountY /*= 1*/, uint32 aGroupCountZ /*= 1*/)
{
	SetResources();
	mD3D12CommandList->Dispatch(aGroupCountX, aGroupCountY, aGroupCountZ);
}

void SR_CommandList_DX12::DrawInstanced(uint32 aVertexCount, uint32 aInstanceCount, uint32 aStartVertex, uint32 aStartInstance)
{
	SetResources();
	mD3D12CommandList->DrawInstanced(aVertexCount, aInstanceCount, aStartVertex, aStartInstance);
}

void SR_CommandList_DX12::DrawIndexedInstanced(uint32 aIndexCount, uint32 aInstanceCount, uint32 aStartIndex, uint32 aStartVertex, uint32 aStartInstance)
{
	SetResources();
	mD3D12CommandList->DrawIndexedInstanced(aIndexCount, aInstanceCount, aStartIndex, aStartVertex, aStartInstance);
}

#if ENABLE_MESH_SHADERS
void SR_CommandList_DX12::DispatchMesh(uint32 aGroupCountX, uint32 aGroupCountY /*= 1*/, uint32 aGroupCountZ /*= 1*/)
{
	if (mD3D12CommandList6)
	{
		SetResources();
		mD3D12CommandList6->DispatchMesh(aGroupCountX, aGroupCountY, aGroupCountZ);
	}
}
#endif

#if ENABLE_RAYTRACING
void SR_CommandList_DX12::DispatchRays(uint32 aThreadCountX, uint32 aThreadCountY /*= 1*/, uint32 aThreadCountZ /*= 1*/)
{
	if (mD3D12CommandList6 && mStateCache.mCurrentShaderState->IsRaytracingShader())
	{
		SetResources();
		D3D12_DISPATCH_RAYS_DESC dispatchRaysDesc = mStateCache.mCurrentShaderState->GetDispatchRaysDesc(); // Get from cached shader state
		dispatchRaysDesc.Width = aThreadCountX;
		dispatchRaysDesc.Height = aThreadCountY;
		dispatchRaysDesc.Depth = aThreadCountZ;

		mD3D12CommandList6->DispatchRays(&dispatchRaysDesc);
	}
}

SC_Ref<SR_BufferResource> SR_CommandList_DX12::CreateAccelerationStructure(const SR_AccelerationStructureInputs& aInputs, SR_BufferResource* aExistingBufferResource)
{
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

	SC_Array<D3D12_RAYTRACING_INSTANCE_DESC> instanceDatas;
	SC_Array<D3D12_RAYTRACING_GEOMETRY_DESC> geometryDescs;
	if (aInputs.mIsTopLevel)
	{
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

		instanceDatas.Reserve(aInputs.mInstanceData->Count());
		for (const SR_RaytracingInstanceData& instance : (*aInputs.mInstanceData))
		{
			D3D12_RAYTRACING_INSTANCE_DESC& instanceData = instanceDatas.Add();
			SC_ZeroMemory(&instanceData, sizeof(D3D12_RAYTRACING_INSTANCE_DESC));

			instanceData.AccelerationStructure = instance.mAccelerationStructureGPUAddress;
			instanceData.InstanceMask = instance.mInstanceMask;
			instanceData.InstanceID = instance.mInstanceId;
			instanceData.InstanceContributionToHitGroupIndex = instance.mHitGroup;

			for (uint32 row = 0; row < 3; ++row)
				for (uint32 col = 0; col < 4; ++col)
					instanceData.Transform[row][col] = instance.mTransform.m44[col][row];

			if (instance.mIsOpaque)
				instanceData.Flags |= D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_OPAQUE;

			switch (instance.mFaceCullingMode)
			{
			case SR_CullMode::Front:
				instanceData.Flags |= D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE;
				break;
			case SR_CullMode::None:
				instanceData.Flags |= D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_CULL_DISABLE;
				break;
			case SR_CullMode::Back:
			default:
				break;
			}
		}

		SR_BufferResourceProperties uploadBufferProps;
		uploadBufferProps.mElementCount = instanceDatas.Count();
		uploadBufferProps.mElementSize = instanceDatas.ElementStride();
		uploadBufferProps.mIsUploadBuffer = true;
		SC_Ref<SR_BufferResource> instanceDataBuffer = SR_RenderDevice::gInstance->CreateBufferResource(uploadBufferProps);
		mTempResources.Add(instanceDataBuffer);
		instanceDataBuffer->UpdateData(0, instanceDatas.GetBuffer(), instanceDatas.GetByteSize());

		inputs.InstanceDescs = instanceDataBuffer->GetGPUAddressStart();
		inputs.NumDescs = instanceDatas.Count();
	}
	else
	{
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;

		for (const SR_RaytracingGeometryData& geoData : (*aInputs.mGeometryData))
		{
			D3D12_RAYTRACING_GEOMETRY_DESC& geometryDesc = geometryDescs.Add();
			SC_ZeroMemory(&geometryDesc, sizeof(D3D12_RAYTRACING_GEOMETRY_DESC));

			geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
			geometryDesc.Triangles.VertexFormat = SR_D3D12ConvertFormat(geoData.mVertexFormat);
			geometryDesc.Triangles.VertexBuffer.StartAddress = geoData.mVertexBuffer->GetGPUAddressStart();
			geometryDesc.Triangles.VertexBuffer.StrideInBytes = geoData.mVertexBuffer->GetProperties().mElementSize;
			geometryDesc.Triangles.VertexCount = geoData.mVertexBuffer->GetProperties().mElementCount;

			geometryDesc.Triangles.IndexFormat = geoData.mIndexBuffer->GetProperties().mElementSize == sizeof(uint16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			geometryDesc.Triangles.IndexBuffer = geoData.mIndexBuffer->GetGPUAddressStart();
			geometryDesc.Triangles.IndexCount = geoData.mIndexBuffer->GetProperties().mElementCount;
		}

		inputs.pGeometryDescs = geometryDescs.GetBuffer();
		inputs.NumDescs = geometryDescs.Count();
	}

	if (aInputs.mFlags & SR_AccelerationStructureInputs::BuildFlag_AllowUpdate)
		inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	if (aInputs.mFlags & SR_AccelerationStructureInputs::BuildFlag_OptimizeBuildSpeed)
		inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD;
	if (aInputs.mFlags & SR_AccelerationStructureInputs::BuildFlag_OptimizeTraceSpeed)
		inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	if (aInputs.mFlags & SR_AccelerationStructureInputs::BuildFlag_OptimizeMemoryFootprint)
		inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY;
	if (aInputs.mFlags & SR_AccelerationStructureInputs::BuildFlag_Update)
		inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
	SR_RenderDevice_DX12::gInstance->GetD3D12Device5()->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);

	SR_BufferResourceProperties bufferResourceProps;
	bufferResourceProps.mElementCount = (uint32)prebuildInfo.ResultDataMaxSizeInBytes;
	bufferResourceProps.mElementSize = 1;
	bufferResourceProps.mBindFlags = SR_BufferBindFlag_RaytracingBuffer;
	bufferResourceProps.mWritable = true;
	bufferResourceProps.mDebugName = "RaytracingAccelerationStructure";

	SC_Ref<SR_BufferResource> bufferResource(aExistingBufferResource);
	if (!bufferResource || (bufferResource->GetProperties().mElementCount < bufferResourceProps.mElementCount))
	{
		if (bufferResource)
			bufferResourceProps.mElementCount = SC_GetNextPow2(bufferResourceProps.mElementCount);

		bufferResource = SR_RenderDevice::gInstance->CreateBufferResource(bufferResourceProps);
	}

	bufferResourceProps.mBindFlags = SR_BufferBindFlag_Buffer;
	bufferResourceProps.mElementCount = SC_GetNextPow2(SC_Max(uint32(64*1024), prebuildInfo.ScratchDataSizeInBytes));
	if (!mAccelerationStructureScratchBuffer || (mAccelerationStructureScratchBuffer->GetProperties().mElementCount < bufferResourceProps.mElementCount))
	{
		mAccelerationStructureScratchBuffer = SR_RenderDevice::gInstance->CreateBufferResource(bufferResourceProps);
	}

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
	desc.DestAccelerationStructureData = bufferResource->GetGPUAddressStart();
	desc.ScratchAccelerationStructureData = mAccelerationStructureScratchBuffer->GetGPUAddressStart();
	desc.Inputs = inputs;

	if (inputs.Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE)
	{
		desc.SourceAccelerationStructureData = aExistingBufferResource->GetGPUAddressStart();
	}

	mD3D12CommandList6->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);
	//UnorderedAccessBarrier();

	return bufferResource;
}
#endif

void SR_CommandList_DX12::ClearRenderTarget(SR_RenderTarget* aRTV, const SC_Vector4& aClearColor)
{
	SR_RenderTarget_DX12* rtv = static_cast<SR_RenderTarget_DX12*>(aRTV);
	if (rtv)
	{
		mD3D12CommandList->ClearRenderTargetView(rtv->GetCPUHandle(), &aClearColor.x, 0, nullptr);
	}
}

void SR_CommandList_DX12::ClearDepthStencil(SR_DepthStencil* aDSV, float aClearValue /*= 0.f*/, uint8 aStencilClearValue /*=0*/, SR_DepthClearFlags aClearFlags /*= SR_DepthClearFlags::All*/)
{
	SR_DepthStencil_DX12* dsv = static_cast<SR_DepthStencil_DX12*>(aDSV);
	if (dsv)
	{
		D3D12_CLEAR_FLAGS clearFlag = {};
		switch (aClearFlags)
		{
		case SR_DepthClearFlags::Depth:
			clearFlag = D3D12_CLEAR_FLAG_DEPTH;
			break;
		case SR_DepthClearFlags::Stencil:
			clearFlag = D3D12_CLEAR_FLAG_STENCIL;
			break;
		default:
			clearFlag = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
			break;
		}
		mD3D12CommandList->ClearDepthStencilView(dsv->GetCPUHandle(), clearFlag, aClearValue, aStencilClearValue, 0, nullptr);
	}
}

void SR_CommandList_DX12::SetShaderState(SR_ShaderState* aShaderState)
{
	SetRootSignature(aShaderState->GetRootSignature());
	SR_ShaderState_DX12* shaderState = static_cast<SR_ShaderState_DX12*>(aShaderState);
#if ENABLE_RAYTRACING
	if (shaderState->IsRaytracingShader())
	{
		mD3D12CommandList6->SetPipelineState1(shaderState->GetD3D12StateObject());
	}
	else
#endif
	{
		mD3D12CommandList->SetPipelineState(shaderState->GetD3D12PipelineState());
	}
	mStateCache.mCurrentShaderState = shaderState;
}

void SR_CommandList_DX12::SetRootSignature(SR_RootSignature* aRootSignature)
{
	const SR_RootSignatureProperties& rootSignatureProperties = aRootSignature->GetProperties();
	const bool isCompute = rootSignatureProperties.mIsCompute;

	SR_RootSignature_DX12* rootSignatureDX12 = static_cast<SR_RootSignature_DX12*>(aRootSignature);
	ID3D12RootSignature* d3d12RootSignature = rootSignatureDX12->GetD3D12RootSignature();

	if (mStateCache.mCurrentRootSignature != rootSignatureDX12)
	{
		ID3D12DescriptorHeap* descriptorHeaps[2];
		descriptorHeaps[0] = static_cast<SR_DescriptorHeap_DX12*>(SR_RenderDevice_DX12::gInstance->GetDefaultDescriptorHeap())->GetD3D12DescriptorHeap();
		descriptorHeaps[1] = static_cast<SR_DescriptorHeap_DX12*>(SR_RenderDevice_DX12::gInstance->GetSamplerDescriptorHeap())->GetD3D12DescriptorHeap();
		mD3D12CommandList->SetDescriptorHeaps(2, descriptorHeaps);

		if (isCompute)
			mD3D12CommandList->SetComputeRootSignature(d3d12RootSignature);
		else
			mD3D12CommandList->SetGraphicsRootSignature(d3d12RootSignature);

		mStateCache.mCurrentRootSignature = rootSignatureDX12;
	}
}

void SR_CommandList_DX12::SetVertexBuffer(SR_BufferResource* aVertexBuffer)
{
	if (aVertexBuffer)
	{
		SR_BufferResource_DX12* vb = static_cast<SR_BufferResource_DX12*>(aVertexBuffer);
		const SR_BufferResourceProperties& props = vb->GetProperties();

		D3D12_VERTEX_BUFFER_VIEW vbView;
		vbView.BufferLocation = vb->GetD3D12Resource()->GetGPUVirtualAddress();
		vbView.SizeInBytes = props.mElementCount * props.mElementSize;
		vbView.StrideInBytes = props.mElementSize;

		mD3D12CommandList->IASetVertexBuffers(0, 1, &vbView);
	}
}

void SR_CommandList_DX12::SetIndexBuffer(SR_BufferResource* aIndexBuffer)
{
	if (aIndexBuffer)
	{
		SR_BufferResource_DX12* ib = static_cast<SR_BufferResource_DX12*>(aIndexBuffer);
		const SR_BufferResourceProperties& props = ib->GetProperties();

		D3D12_INDEX_BUFFER_VIEW ibView;
		ibView.BufferLocation = ib->GetD3D12Resource()->GetGPUVirtualAddress();
		ibView.SizeInBytes = props.mElementCount * props.mElementSize;
		ibView.Format = (props.mElementSize == sizeof(uint16)) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

		mD3D12CommandList->IASetIndexBuffer(&ibView);
	}
}

void SR_CommandList_DX12::SetPrimitiveTopology(const SR_PrimitiveTopology& aPrimitiveTopology)
{
	switch (aPrimitiveTopology)
	{
	case SR_PrimitiveTopology::PointList:
		mD3D12CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	case SR_PrimitiveTopology::LineList:
		mD3D12CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case SR_PrimitiveTopology::LineStrip:
		mD3D12CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
		break;
	case SR_PrimitiveTopology::TriangleList:
	default:
		mD3D12CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	}
}

void SR_CommandList_DX12::SetRootConstant()
{

}

void SR_CommandList_DX12::SetRootConstantBuffer(SR_BufferResource* aConstantBuffer, uint32 aSlot)
{
	SC_ASSERT(aSlot < 4);
	if (mResourceBindings.mConstantBuffers[aSlot] != aConstantBuffer)
	{
		mResourceBindings.mConstantBuffers[aSlot] = aConstantBuffer;
		mResourceBindings.mConstantsDirty[aSlot] = true;
	}
}

void SR_CommandList_DX12::SetResourceInfo(uint8* /*aData*/, uint32 /*aSize*/)
{


}

void SR_CommandList_DX12::SetRootShaderResource()
{

}

void SR_CommandList_DX12::SetRootUnorderedAccessResource()
{

}

void SR_CommandList_DX12::SetTexture(SR_Texture* aTexture, uint32 aSlot)
{
	SC_ASSERT(aSlot < 1);
	if (mResourceBindings.mTextures[aSlot] != aTexture)
	{
		mResourceBindings.mTextures[aSlot] = aTexture;
		mResourceBindings.mTexturesDirty[aSlot] = true;
	}
}

void SR_CommandList_DX12::SetBuffer(SR_Buffer* /*aBuffer*/, uint32 /*aSlot*/)
{
}

void SR_CommandList_DX12::SetRenderTargets(uint32 aNumRTVs, SR_RenderTarget** aRTVs, SR_DepthStencil* aDSV)
{
	SC_Array<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
	rtvHandles.Reserve(SR_MAX_RENDER_TARGETS);
	for (uint32 i = 0; i < aNumRTVs; ++i)
	{
		if (aRTVs[i])
		{
			SR_RenderTarget_DX12* rtv = static_cast<SR_RenderTarget_DX12*>(aRTVs[i]);
			rtvHandles.Add(rtv->GetCPUHandle());
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {};
	if (aDSV)
	{
		SR_DepthStencil_DX12* dsv = static_cast<SR_DepthStencil_DX12*>(aDSV);
		dsvHandle = dsv->GetCPUHandle();
	}

	mD3D12CommandList->OMSetRenderTargets(rtvHandles.Count(), rtvHandles.GetBuffer(), false, (aDSV) ? &dsvHandle : nullptr);
}

void SR_CommandList_DX12::SetBlendFactor(const SC_Vector4& aBlendFactor)
{
	mD3D12CommandList->OMSetBlendFactor(&aBlendFactor.x);
}

void SR_CommandList_DX12::TransitionBarrier(const SC_Array<SC_Pair<uint32, SR_Resource*>>& aTransitions)
{
	SC_Array<D3D12_RESOURCE_BARRIER> barriers;

	for (uint32 i = 0; i < aTransitions.Count(); ++i)
	{
		const SC_Pair<uint32, SR_Resource*>& currentResource = aTransitions[i];

		D3D12_RESOURCE_BARRIER barrier = {}; 
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		barrier.Transition.pResource = currentResource.mSecond->mTrackedD3D12Resource;
		barrier.Transition.StateBefore = SR_D3D12ConvertResourceState(currentResource.mSecond->mLatestResourceState);
		barrier.Transition.StateAfter = SR_D3D12ConvertResourceState(currentResource.mFirst);
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		currentResource.mSecond->mLatestResourceState = currentResource.mFirst;

		if (barrier.Transition.StateBefore != barrier.Transition.StateAfter)
			barriers.Add(SC_Move(barrier));
	}

	if (barriers.Count())
	{
		mD3D12CommandList->ResourceBarrier(barriers.Count(), barriers.GetBuffer());
	}
}

void SR_CommandList_DX12::UnorderedAccessBarrier(SR_Resource* aResource)
{
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = aResource->mTrackedD3D12Resource;

	mD3D12CommandList->ResourceBarrier(1, &barrier);
}

void SR_CommandList_DX12::AliasBarrier()
{

}

void SR_CommandList_DX12::CopyResource(SR_Resource* aDstResource, SR_Resource* aSrcResource)
{
	SC_ASSERT((aDstResource != nullptr) && (aSrcResource != nullptr) && "Resources cannot be nullptr.");
	if (aDstResource->mTrackedD3D12Resource != aSrcResource->mTrackedD3D12Resource)
	{
		mD3D12CommandList->CopyResource(aDstResource->mTrackedD3D12Resource, aSrcResource->mTrackedD3D12Resource);
	}
}

void SR_CommandList_DX12::CopyBuffer(SR_BufferResource* aDstBuffer, uint32 aDstOffset, SR_BufferResource* aSrcBuffer, uint32 aSrcOffset, uint32 aSize)
{
	SC_ASSERT(aDstBuffer != aSrcBuffer);
	SR_BufferResource_DX12* dstBuffer = static_cast<SR_BufferResource_DX12*>(aDstBuffer);
	SR_BufferResource_DX12* srcBuffer = static_cast<SR_BufferResource_DX12*>(aSrcBuffer);

	mD3D12CommandList->CopyBufferRegion(dstBuffer->GetD3D12Resource(), aDstOffset, srcBuffer->GetD3D12Resource(), aSrcOffset, aSize);
}

void SR_CommandList_DX12::CopyTexture(SR_TextureResource* aDstTexture, SR_TextureResource* aSrcResource)
{
	SC_ASSERT(aDstTexture != aSrcResource);

	SR_TextureResource_DX12* dstTexture = static_cast<SR_TextureResource_DX12*>(aDstTexture);
	SR_TextureResource_DX12* srcTexture = static_cast<SR_TextureResource_DX12*>(aSrcResource);

	ID3D12Resource* dstResource = dstTexture->GetD3D12Resource();
	ID3D12Resource* srcResource = srcTexture->GetD3D12Resource();
	if (dstResource != srcResource)
	{
		D3D12_TEXTURE_COPY_LOCATION destLoc = dstTexture->GetCopyLocation(0);
		D3D12_TEXTURE_COPY_LOCATION srcLoc = srcTexture->GetCopyLocation(0);
		mD3D12CommandList->CopyTextureRegion(&destLoc, 0, 0, 0, &srcLoc, nullptr);
	}
}

void SR_CommandList_DX12::UpdateBuffer(SR_BufferResource* aDstBuffer, uint32 aDstOffset, const void* aData, uint32 aSize)
{
	//SR_BufferResource_DX12* dstBuffer = static_cast<SR_BufferResource_DX12*>(aDstBuffer);

	// Create temp buffer for copying
	SR_BufferResourceProperties tempBufferProps;
	tempBufferProps.mElementSize = 1;
	tempBufferProps.mElementCount = aSize;
	tempBufferProps.mBindFlags = SR_BufferBindFlag_Staging;
	tempBufferProps.mIsUploadBuffer = true;
	SC_Ref<SR_BufferResource> tempBuffer = SR_RenderDevice_DX12::gInstance->CreateBufferResource(tempBufferProps); // Can be a temp resource?
	mTempResources.Add(tempBuffer);

	tempBuffer->UpdateData(aDstOffset, aData, aSize);

	CopyBuffer(aDstBuffer, aDstOffset, tempBuffer, 0, aSize);
}

void SR_CommandList_DX12::UpdateTexture(SR_TextureResource* aTextureResource, const SR_PixelData* aData, uint32 aDataCount, bool /*aKeepData*/)
{
	if (!aDataCount)
		return;

	SR_TextureResource_DX12* resourceDX12 = static_cast<SR_TextureResource_DX12*>(aTextureResource);
	const SR_TextureResourceProperties& resourceProperties = aTextureResource->GetProperties();

	bool singleUpdates = (aDataCount == 1);

	if (SR_GetNumSubresources(resourceProperties) == aDataCount)
	{
		UpdateTextureInternal(resourceDX12, SC_IntVector(0), SC_IntVector(0), resourceProperties, aData, aDataCount, 0);
	}
	else if (!singleUpdates)
	{
		uint32 minSubresourceIndex = UINT32_MAX;
		uint32 maxSubresourceIndex = 0;
		for (uint32 i = 0; i < aDataCount; ++i)
		{
			const SR_PixelData& data = aData[i];
			uint32 subresourceIndex = SR_GetD3D12SubresourceIndex(resourceProperties, data.mLevel);
			minSubresourceIndex = SC_Min(subresourceIndex, minSubresourceIndex);
			maxSubresourceIndex = SC_Max(subresourceIndex, maxSubresourceIndex);
		}
		SC_ASSERT(maxSubresourceIndex >= minSubresourceIndex);

		uint32 numSubresources = maxSubresourceIndex - minSubresourceIndex + 1;
		if (numSubresources == aDataCount)
			UpdateTextureInternal(resourceDX12, SC_IntVector(0), SC_IntVector(0), resourceProperties, aData, aDataCount, minSubresourceIndex);
		else
			singleUpdates = true;
	}
	
	if (singleUpdates)
	{
		for (uint32 i = 0; i < aDataCount; ++i)
		{
			const SR_PixelData& data = aData[i];
			uint32 subresourceIndex = SR_GetD3D12SubresourceIndex(resourceProperties, data.mLevel);
			UpdateTextureInternal(resourceDX12, SC_IntVector(0), SC_IntVector(0), resourceProperties, &data, 1, subresourceIndex);
		}
	}
}

void SR_CommandList_DX12::UpdateTexture(SR_TextureResource* aTextureResource, const SC_IntVector& aDstOffset, const SC_IntVector& aSize, const SR_PixelData& aData, bool /*aKeepData*/)
{
	SR_TextureResource_DX12* resourceDX12 = static_cast<SR_TextureResource_DX12*>(aTextureResource);
	const SR_TextureResourceProperties& resourceProperties = aTextureResource->GetProperties();

	SR_TextureResourceProperties props;
	props.mType = resourceProperties.mType;
	props.mIsCubeMap = resourceProperties.mIsCubeMap;
	props.mFormat = resourceProperties.mFormat;
	props.mSize = aSize;

	uint32 blockSize = SR_GetFormatBlockSize(resourceProperties.mFormat);
	props.mSize.x = SC_Max(uint32(props.mSize.x), blockSize);
	props.mSize.y = SC_Max(uint32(props.mSize.y), blockSize);

	UpdateTextureInternal(resourceDX12, aDstOffset, props.mSize, props, &aData, 1, 0);
}

void SR_CommandList_DX12::SetViewport(const SR_Rect& aRect, float aMinDepth /*= 0.0f*/, float aMaxDepth /*= 1.0f*/)
{
	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = (float)aRect.mLeft;
	viewport.TopLeftY = (float)aRect.mTop;
	viewport.Width = (float)aRect.mRight - aRect.mLeft;
	viewport.Height = (float)aRect.mBottom - aRect.mTop;
	viewport.MinDepth = aMinDepth;
	viewport.MaxDepth = aMaxDepth;

	mD3D12CommandList->RSSetViewports(1, &viewport);
}

void SR_CommandList_DX12::SetScissorRect(const SR_Rect& aRect)
{
	D3D12_RECT scissorRect = {};
	scissorRect.left = aRect.mLeft;
	scissorRect.top = aRect.mTop;
	scissorRect.right = aRect.mRight;
	scissorRect.bottom = aRect.mBottom;

	mD3D12CommandList->RSSetScissorRects(1, &scissorRect);
}

ID3D12GraphicsCommandList* SR_CommandList_DX12::GetD3D12CommandList() const
{
    return mD3D12CommandList.Get();
}

void SR_CommandList_DX12::UpdateTextureInternal(SR_TextureResource_DX12* aTextureResource, const SC_IntVector& aDstOffset, const SC_IntVector& aSize, const SR_TextureResourceProperties& aCopyTextureProperties, const SR_PixelData* aData, uint32 aDataCount, uint32 aFirstSubresource)
{
	if (!aData)
		return;

	SC_Array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footPrints;
	footPrints.Respace(aDataCount);

	bool isSingle = aDataCount == 1;

	D3D12_RESOURCE_DESC tempDesc = SR_GetD3D12ResourceDesc(aCopyTextureProperties);
	uint64 tempBufferSize = 0;
	SR_RenderDevice_DX12::gInstance->GetD3D12Device()->GetCopyableFootprints(&tempDesc, aFirstSubresource, aDataCount, 0, footPrints.GetBuffer(), nullptr, nullptr, &tempBufferSize);

	SR_BufferResourceProperties tempBufferProps;
	tempBufferProps.mElementSize = 1;
	tempBufferProps.mElementCount = static_cast<uint32>(tempBufferSize);
	tempBufferProps.mBindFlags = SR_BufferBindFlag_Staging;
	tempBufferProps.mIsUploadBuffer = true;

	SC_Ref<SR_BufferResource> tempBuf = SR_RenderDevice_DX12::gInstance->CreateBufferResource(tempBufferProps); // Can be a temp resource?
	mTempResources.Add(tempBuf);
	SR_BufferResource_DX12* tempBuffer = static_cast<SR_BufferResource_DX12*>(tempBuf.Get());
	void* dataPtr = tempBuffer->GetDataPtr();

	D3D12_TEXTURE_COPY_LOCATION dstLoc = { aTextureResource->GetD3D12Resource() };
	dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	D3D12_TEXTURE_COPY_LOCATION srcLoc = { tempBuffer->GetD3D12Resource() };
	srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	srcLoc.PlacedFootprint.Offset = 0;

	D3D12_BOX srcBox = {};
	srcBox.right = (uint32)aSize.x;
	srcBox.bottom = (uint32)aSize.y;
	srcBox.back = (uint32)aSize.z;

	for (uint32 i = 0; i < aDataCount; ++i)
	{
		const SR_PixelData& data = aData[i];
		dstLoc.SubresourceIndex = SR_GetD3D12SubresourceIndex(aTextureResource->GetProperties(), data.mLevel);
		srcLoc.PlacedFootprint = footPrints[isSingle ? 0 : dstLoc.SubresourceIndex - aFirstSubresource];
		SC_ASSERT(srcLoc.PlacedFootprint.Offset != UINT64_MAX);

		SR_MemcpyPixelData(dataPtr, srcLoc.PlacedFootprint, data, aTextureResource->GetProperties().mFormat);
		mD3D12CommandList->CopyTextureRegion(&dstLoc, aDstOffset.x, aDstOffset.y, aDstOffset.z, &srcLoc, aSize.x ? &srcBox : nullptr);
	}
}

void SR_CommandList_DX12::SetResources()
{
	if (mStateCache.mCurrentRootSignature)
	{
		bool isTexturesDirty = mResourceBindings.mTexturesDirty.any();
		bool isBuffersDirty = false;
		bool isConstantsDirty = mResourceBindings.mConstantsDirty.any();
		bool needsResourceRefresh = (isTexturesDirty || isBuffersDirty || isConstantsDirty);

		if (!needsResourceRefresh)
			return;

		const SR_RootSignatureProperties& rootSignatureProperties = mStateCache.mCurrentRootSignature->GetProperties();

		uint32 paramIndex = 0;
		for (const SR_RootParam& param : rootSignatureProperties.mRootParams)
		{
			if (param.mType == SR_RootParamType::CBV && isConstantsDirty)
			{
				SR_BufferResource_DX12* cb = static_cast<SR_BufferResource_DX12*>(mResourceBindings.mConstantBuffers[param.mDescriptor.mRegisterIndex]);

				if (!cb || !mResourceBindings.mConstantsDirty[param.mDescriptor.mRegisterIndex])
				{
					++paramIndex;
					continue;
				}

				if (rootSignatureProperties.mIsCompute)
					mD3D12CommandList->SetComputeRootConstantBufferView(paramIndex, cb->GetD3D12Resource()->GetGPUVirtualAddress());
				else
					mD3D12CommandList->SetGraphicsRootConstantBufferView(paramIndex, cb->GetD3D12Resource()->GetGPUVirtualAddress());
			}
			else if (param.mType == SR_RootParamType::SRV)
			{
				D3D12_GPU_VIRTUAL_ADDRESS address = {};

				if (rootSignatureProperties.mIsCompute)
					mD3D12CommandList->SetComputeRootShaderResourceView(paramIndex, address);
				else
					mD3D12CommandList->SetGraphicsRootShaderResourceView(paramIndex, address);
			}

			++paramIndex;
		}
	}
}

#endif