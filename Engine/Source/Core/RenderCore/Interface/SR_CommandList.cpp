#include "SR_CommandList.h"

SR_CommandList::SR_CommandList(const SR_CommandListType& aType)
	: mType(aType)
{

}

SR_CommandList::~SR_CommandList()
{
}

void SR_CommandList::Begin()
{
	mTempResources.RemoveAll();
	mFenceWaits.RemoveAll();
}

void SR_CommandList::End()
{

}

void SR_CommandList::BeginEvent(const char* /*aName*/)
{
}

void SR_CommandList::EndEvent()
{
}

void SR_CommandList::Dispatch(SR_ShaderState* aShader, uint32 aGroupCountX, uint32 aGroupCountY, uint32 aGroupCountZ)
{
	Dispatch(aShader, SC_IntVector(aGroupCountX, aGroupCountY, aGroupCountZ));
}

void SR_CommandList::Dispatch(SR_ShaderState* aShader, const SC_IntVector& aThreadGroups)
{
	if (!aShader->IsComputeShader())
	{
		assert(false);
		return;
	}

	SC_IntVector actualThreadGroups;
	const SC_IntVector& numThreads = aShader->GetMetaData(SR_ShaderType::Compute).mNumThreads;
	actualThreadGroups.x = SC_Max((aThreadGroups.x + numThreads.x - 1) / numThreads.x, 1);
	actualThreadGroups.y = SC_Max((aThreadGroups.y + numThreads.y - 1) / numThreads.y, 1);
	actualThreadGroups.z = SC_Max((aThreadGroups.z + numThreads.z - 1) / numThreads.z, 1);
	SetShaderState(aShader);
	Dispatch(actualThreadGroups);
}

void SR_CommandList::Dispatch(const SC_IntVector3& aThreadGroups)
{
	Dispatch((uint32)aThreadGroups.x, (uint32)aThreadGroups.y, (uint32)aThreadGroups.z);
}

void SR_CommandList::Dispatch(uint32 /*aGroupCountX*/, uint32 /*aGroupCountY*/ /*= 1*/, uint32 /*aGroupCountZ*/ /*= 1*/)
{

}

void SR_CommandList::Draw(uint32 aVertexCount, uint32 aStartVertex)
{
	DrawInstanced(aVertexCount, 1, aStartVertex, 0);
}

void SR_CommandList::DrawInstanced(uint32 /*aVertexCount*/, uint32 /*aInstanceCount*/, uint32 /*aStartVertex*/, uint32 /*aStartInstance*/)
{
}

void SR_CommandList::DrawIndexed(uint32 aIndexCount, uint32 aStartIndex, uint32 aStartVertex)
{
	DrawIndexedInstanced(aIndexCount, 1, aStartIndex, aStartVertex, 0);
}

void SR_CommandList::DrawIndexedInstanced(uint32 /*aIndexCount*/, uint32 /*aInstanceCount*/, uint32 /*aStartIndex*/, uint32 /*aStartVertex*/, uint32 /*aStartInstance*/)
{
}

#if ENABLE_MESH_SHADERS

void SR_CommandList::DispatchMesh(const SC_IntVector3& aThreadGroups)
{
	DispatchMesh(aThreadGroups.x, aThreadGroups.y, aThreadGroups.z);
}

void SR_CommandList::DispatchMesh(uint32 /*aGroupCountX*/, uint32 /*aGroupCountY*/ /*= 1*/, uint32 /*aGroupCountZ*/ /*= 1*/)
{

}
#endif

#if ENABLE_RAYTRACING
void SR_CommandList::DispatchRays(const SC_IntVector& aThreadCounts)
{
	DispatchRays(aThreadCounts.x, aThreadCounts.y, aThreadCounts.z);
}
void SR_CommandList::DispatchRays(uint32 /*aThreadCountX*/, uint32 /*aThreadCountY*/ /*= 1*/, uint32 /*aThreadCountZ*/ /*= 1*/)
{

}

SC_Ref<SR_BufferResource> SR_CommandList::CreateAccelerationStructure(const SR_AccelerationStructureInputs& /*aInputs*/, SR_BufferResource* /*aExistingBufferResource*/)
{
	return nullptr;
}

SC_Ref<SR_Buffer> SR_CommandList::BuildRaytracingBuffer(const SC_Array<SR_RaytracingInstanceData>& aInstances, SR_Buffer* aExistingBuffer)
{
	SR_AccelerationStructureInputs inputs;
	inputs.mIsTopLevel = true;
	inputs.mInstanceData = &aInstances;

	SR_BufferResource* existingBufferResource = (aExistingBuffer) ? aExistingBuffer->GetResource() : nullptr;
	SC_Ref<SR_BufferResource> buffer = CreateAccelerationStructure(inputs, existingBufferResource);
	mTempResources.Add(buffer);

	SR_BufferProperties bufferProps;
	bufferProps.mType = SR_BufferType::RaytracingBuffer;
	bufferProps.mElementCount = buffer->GetProperties().mElementCount;

	return SR_RenderDevice::gInstance->CreateBuffer(bufferProps, buffer);
}
#endif

void SR_CommandList::ClearRenderTarget(SR_RenderTarget* /*aRTV*/, const SC_Vector4& /*aClearColor*/)
{

}

void SR_CommandList::ClearRenderTargets(uint32 aNumRTVs, SR_RenderTarget** aRTVs, const SC_Vector4& aClearColor /*= SC_Vector4(0.f)*/)
{
	for (uint32 i = 0; i < aNumRTVs; ++i)
	{
		ClearRenderTarget(aRTVs[i], aClearColor);
	}
}

void SR_CommandList::ClearDepthStencil(SR_DepthStencil* /*aDSV*/, float /*aClearValue*/ /*= 0.f*/, uint8 /* aStencilClearValue*/ /*=0*/, SR_DepthClearFlags /*aClearFlags*/ /*= SR_DepthClearFlags::All*/)
{

}

void SR_CommandList::SetShaderState(SR_ShaderState* /*aShaderState*/)
{

}

void SR_CommandList::SetRootSignature(SR_RootSignature* /*aRootSignature*/)
{

}

void SR_CommandList::SetVertexBuffer(SR_BufferResource* /*aVertexBuffer*/)
{
}

void SR_CommandList::SetIndexBuffer(SR_BufferResource* /*aIndexBuffer*/)
{
}

void SR_CommandList::SetPrimitiveTopology(const SR_PrimitiveTopology& /*aPrimitiveTopology*/)
{
}

void SR_CommandList::SetRootConstant()
{

}

void SR_CommandList::SetRootConstantBuffer(SR_BufferResource* /*aConstantBuffer*/, uint32 /*aSlot*/)
{

}

void SR_CommandList::SetResourceInfo(uint8* /*aData*/, uint32 /*aSize*/)
{
}

void SR_CommandList::SetRootShaderResource()
{

}

void SR_CommandList::SetRootUnorderedAccessResource()
{

}

void SR_CommandList::SetTexture(SR_Texture* /*aTexture*/, uint32 /*aSlot*/)
{
}

void SR_CommandList::SetBuffer(SR_Buffer* /*aBuffer*/, uint32 /*aSlot*/)
{
}

void SR_CommandList::SetRenderTarget(SR_RenderTarget* aRTV, SR_DepthStencil* aDSV)
{
	SetRenderTargets(1, &aRTV, aDSV);
}

void SR_CommandList::SetRenderTargets(uint32 /*aNumRTVs*/, SR_RenderTarget** /*aRTVs*/, SR_DepthStencil* /*aDSV*/)
{

}

void SR_CommandList::SetBlendFactor(const SC_Vector4& /*aBlendFactor*/)
{
}

void SR_CommandList::TransitionBarrier(uint32 aTargetResourceState, SR_Resource* aResource)
{
	SC_Array<SC_Pair<uint32, SR_Resource*>> pairs;
	pairs.Add(SC_Pair(aTargetResourceState, aResource));
	TransitionBarrier(pairs);
}

void SR_CommandList::TransitionBarrier(const SC_Array<SC_Pair<uint32, SR_Resource*>>& /*aTransitions*/)
{

}

void SR_CommandList::UnorderedAccessBarrier(SR_Resource* /*aResource*/)
{

}

void SR_CommandList::AliasBarrier()
{

}

void SR_CommandList::CopyResource(SR_Resource* /*aDstResource*/, SR_Resource* /*aSrcResource*/)
{

}

void SR_CommandList::CopyBuffer(SR_BufferResource* /*aDstBuffer*/, uint32 /*aDstOffset*/, SR_BufferResource* /*aSrcBuffer*/, uint32 /*aSrcOffset*/, uint32 /*aSize*/)
{
}

void SR_CommandList::CopyTexture(SR_TextureResource* /*aDstTexture*/, SR_TextureResource* /*aSrcResource*/)
{
}

void SR_CommandList::UpdateBuffer(SR_BufferResource* /*aDstBuffer*/, uint32 /*aDstOffset*/, const void* /*aData*/, uint32 /*aSize*/)
{
}

void SR_CommandList::UpdateTexture(SR_TextureResource* /*aTextureResource*/, const SR_PixelData* /*aData*/, uint32 /*aDataCount*/, bool /*aKeepData*/)
{
}

void SR_CommandList::UpdateTexture(SR_TextureResource* /*aTextureResource*/, const SC_IntVector& /*aDstOffset*/, const SC_IntVector& /*aSize*/, const SR_PixelData& /*aData*/, bool /*aKeepData*/)
{
}

void SR_CommandList::SetViewport(const SR_Rect& /*aRect*/, float /*aMinDepth = 0.0f*/, float /*aMaxDepth = 1.0f*/)
{

}

void SR_CommandList::SetScissorRect(const SR_Rect& /*aRect*/)
{
}

void SR_CommandList::WaitFor(SR_TaskEvent* aEvent)
{
	if (!aEvent)
		return;

	aEvent->mCPUEvent.Wait();

	// Commands going on the same queue never needs to wait for each other.
	if (aEvent->mFence.mType == mType)
		return;

	mFenceWaits.Add(aEvent->mFence);
}

void SR_CommandList::WaitFor(const SC_UniquePtr<SR_TaskEvent>& aEvent)
{
	WaitFor(aEvent.get());
}

const SC_Array<SR_Fence>& SR_CommandList::GetFenceWaits() const
{
	return mFenceWaits;
}