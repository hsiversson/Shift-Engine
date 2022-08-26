#include "SR_CommandList_Vk.h"

#if SR_ENABLE_VULKAN

SR_CommandList_Vk::SR_CommandList_Vk(const SR_CommandListType& aType)
	: SR_CommandList(aType)
{

}

SR_CommandList_Vk::~SR_CommandList_Vk()
{

}

void SR_CommandList_Vk::Begin()
{
}

void SR_CommandList_Vk::End()
{

}

bool SR_CommandList_Vk::IsClosed() const
{

}

void SR_CommandList_Vk::BeginEvent(const char* /*aName*/)
{
}

void SR_CommandList_Vk::EndEvent()
{

}

void SR_CommandList_Vk::Dispatch(uint32 aGroupCountX, uint32 aGroupCountY /*= 1*/, uint32 aGroupCountZ /*= 1*/)
{
	SetResources();
	vkCmdDispatch(mCommandBuffer, aGroupCountX, aGroupCountY, aGroupCountZ);
}

void SR_CommandList_Vk::DrawInstanced(uint32 aVertexCount, uint32 aInstanceCount, uint32 aStartVertex /*= 0*/, uint32 aStartInstance /*= 0*/)
{
	SetResources();
	vkCmdDraw(mCommandBuffer, aVertexCount, aInstanceCount, aStartVertex, aStartInstance);
}

void SR_CommandList_Vk::DrawIndexedInstanced(uint32 aIndexCount, uint32 aInstanceCount, uint32 aStartIndex /*= 0*/, uint32 aStartVertex /*= 0*/, uint32 aStartInstance /*= 0*/)
{
	SetResources();
	vkCmdDrawIndexed(mCommandBuffer, aIndexCount, aInstanceCount, aStartIndex, aStartVertex, aStartInstance);
}

#if SR_ENABLE_MESH_SHADERS
void SR_CommandList_Vk::DispatchMesh(uint32 /*aGroupCountX*/, uint32 /*aGroupCountY*/ /*= 1*/, uint32 /*aGroupCountZ*/ /*= 1*/)
{
	SetResources();
	//vkCmdDrawMeshTasksNV(mCommandBuffer, );
}
#endif //SR_ENABLE_MESH_SHADERS

#if SR_ENABLE_RAYTRACING
void SR_CommandList_Vk::DispatchRays(uint32 aThreadCountX, uint32 aThreadCountY /*= 1*/, uint32 aThreadCountZ /*= 1*/)
{
	SetResources();
	vkCmdTraceRaysKHR(mCommandBuffer, nullptr, nullptr, nullptr, nullptr, aThreadCountX, aThreadCountY, aThreadCountZ);
}

SC_Ref<SR_BufferResource> SR_CommandList_Vk::CreateAccelerationStructure(const SR_AccelerationStructureInputs& /*aInputs*/, SR_BufferResource* /*aExistingBufferResource*/)
{
	vkCmdBuildAccelerationStructuresKHR(mCommandBuffer, 0, nullptr, nullptr);
	return nullptr;
}
#endif //SR_ENABLE_RAYTRACING

void SR_CommandList_Vk::ClearRenderTarget(SR_RenderTarget* /*aRTV*/, const SC_Vector4& /*aClearColor*/)
{

}

void SR_CommandList_Vk::ClearDepthStencil(SR_DepthStencil* /*aDSV*/, float /*aClearValue*/ /*= 0.f*/, uint8 /*aStencilClearValue*/ /*= 0*/, SR_DepthClearFlags /*aClearFlags*/ /*= SR_DepthClearFlags::All*/)
{

}

void SR_CommandList_Vk::SetShaderState(SR_ShaderState* aShaderState)
{
	VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	if (aShaderState->IsRaytracingShader())
		bindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
	else if (aShaderState->IsComputeShader())
		bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

	VkPipeline pipeline;
	vkCmdBindPipeline(mCommandBuffer, bindPoint, pipeline);
}

void SR_CommandList_Vk::SetRootSignature(SR_RootSignature* /*aRootSignature*/)
{

}

void SR_CommandList_Vk::SetVertexBuffer(SR_BufferResource* /*aVertexBuffer*/)
{

}

void SR_CommandList_Vk::SetIndexBuffer(SR_BufferResource* /*aIndexBuffer*/)
{

}

void SR_CommandList_Vk::SetPrimitiveTopology(const SR_PrimitiveTopology& /*aPrimitiveTopology*/)
{

}

void SR_CommandList_Vk::SetRootConstant()
{
}

void SR_CommandList_Vk::SetRootConstantBuffer(SR_BufferResource* /*aConstantBuffer*/, uint32 /*aSlot*/)
{

}

void SR_CommandList_Vk::SetRootConstantBuffer(SR_BufferResource* /*aConstantBuffer*/, uint64 /*aBufferOffset*/, uint32 /*aSlot*/)
{

}

void SR_CommandList_Vk::SetResourceInfo(uint8* /*aData*/, uint32 /*aSize*/)
{

}

void SR_CommandList_Vk::SetRootShaderResource()
{

}

void SR_CommandList_Vk::SetRootUnorderedAccessResource()
{

}

void SR_CommandList_Vk::SetTexture(SR_Texture* /*aTexture*/, uint32 /*aSlot*/)
{

}

void SR_CommandList_Vk::SetBuffer(SR_Buffer* /*aBuffer*/, uint32 /*aSlot*/)
{

}

void SR_CommandList_Vk::SetRenderTargets(uint32 /*aNumRTVs*/, SR_RenderTarget** /*aRTVs*/, SR_DepthStencil* /*aDSV*/)
{

}

void SR_CommandList_Vk::SetBlendFactor(const SC_Vector4& /*aBlendFactor*/)
{

}

void SR_CommandList_Vk::TransitionBarrier(const SC_Array<SC_Pair<uint32, SR_TrackedResource*>>& /*aTransitions*/)
{

}

void SR_CommandList_Vk::UnorderedAccessBarrier(SR_TrackedResource* /*aResource*/)
{

}

void SR_CommandList_Vk::AliasBarrier()
{

}

void SR_CommandList_Vk::CopyResource(SR_TrackedResource* /*aDstResource*/, SR_TrackedResource* /*aSrcResource*/)
{

}

void SR_CommandList_Vk::CopyBuffer(SR_BufferResource* /*aDstBuffer*/, uint64 /*aDstOffset*/, SR_BufferResource* /*aSrcBuffer*/, uint64 /*aSrcOffset*/, uint32 /*aSize*/)
{

}

void SR_CommandList_Vk::CopyTexture(SR_TextureResource* /*aDstTexture*/, SR_TextureResource* /*aSrcResource*/)
{

}

void SR_CommandList_Vk::UpdateBuffer(SR_BufferResource* /*aDstBuffer*/, uint32 /*aDstOffset*/, const void* /*aData*/, uint32 /*aSize*/)
{

}

void SR_CommandList_Vk::UpdateTexture(SR_TextureResource* /*aTextureResource*/, const SR_PixelData* /*aData*/, uint32 /*aDataCount*/, bool /*aKeepData*/)
{

}

void SR_CommandList_Vk::UpdateTexture(SR_TextureResource* /*aTextureResource*/, const SC_IntVector& /*aDstOffset*/, const SC_IntVector& /*aSize*/, const SR_PixelData& /*aData*/, bool /*aKeepData*/)
{

}

SR_BufferResource* SR_CommandList_Vk::GetBufferResource(uint64& /*aOutOffset*/, SR_BufferBindFlag /*aBufferType*/, uint32 /*aByteSize*/, void* /*aInitialData*/, uint32 /*aAlignment*/ /*= 0*/, const SR_Fence& /*aCompletionFence*/ /*= SR_Fence()*/)
{
	return nullptr;
}

SR_Buffer* SR_CommandList_Vk::GetBuffer()
{
	return nullptr;
}

void SR_CommandList_Vk::SetViewport(const SR_Rect& /*aRect*/, float /*aMinDepth*/ /*= 0.0f*/, float /*aMaxDepth*/ /*= 1.0f*/)
{
	VkViewport viewport;
	vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);
}

void SR_CommandList_Vk::SetScissorRect(const SR_Rect& /*aRect*/)
{
	VkRect2D scissorRect;	
	vkCmdSetScissor(mCommandBuffer, 0, 1, &scissorRect);
}

void SR_CommandList_Vk::SetResources()
{
}

#endif