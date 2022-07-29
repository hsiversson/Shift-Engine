#pragma once
#include "SR_RenderTypes.h"
#include "SR_RaytracingStructs.h"
#include "RenderCore/Profiling/SR_GPUProfiler.h"
#include "RenderCore/Resources/SR_RingBuffer.h"
#include <bitset>

class SR_ShaderState;
class SR_RenderTarget;
class SR_DepthStencil;
class SR_Buffer;
class SR_BufferResource;
class SR_RootSignature;
class SR_Texture;
class SR_Resource;
class SR_TextureResource;
struct SR_Fence;
struct SR_TaskEvent;
struct SR_PixelData;

enum class SR_DepthClearFlags
{
	Depth,
	Stencil,
	All,
};

class SR_CommandList
{
public:
	SR_CommandList(const SR_CommandListType& aType);
	virtual ~SR_CommandList();

	virtual void Begin();
	virtual void End();

	virtual void BeginEvent(const char* aName);
	virtual void EndEvent();

	// Draw/Dispatch
	void Dispatch(SR_ShaderState* aShader, uint32 aGroupCountX, uint32 aGroupCountY = 1, uint32 aGroupCountZ = 1);
	void Dispatch(SR_ShaderState* aShader, const SC_IntVector& aThreadGroups);
	void Dispatch(const SC_IntVector3& aThreadGroups);
	virtual void Dispatch(uint32 aGroupCountX, uint32 aGroupCountY = 1, uint32 aGroupCountZ = 1);

	void Draw(uint32 aVertexCount, uint32 aStartVertex = 0);
	virtual void DrawInstanced(uint32 aVertexCount, uint32 aInstanceCount, uint32 aStartVertex = 0, uint32 aStartInstance = 0);
	void DrawIndexed(uint32 aIndexCount, uint32 aStartIndex = 0, uint32 aStartVertex = 0);
	virtual void DrawIndexedInstanced(uint32 aIndexCount, uint32 aInstanceCount, uint32 aStartIndex = 0, uint32 aStartVertex = 0, uint32 aStartInstance = 0);

#if SR_ENABLE_MESH_SHADERS
	void DispatchMesh(const SC_IntVector3& aThreadGroups);
	virtual void DispatchMesh(uint32 aGroupCountX, uint32 aGroupCountY = 1, uint32 aGroupCountZ = 1);
#endif

#if SR_ENABLE_RAYTRACING
	void DispatchRays(const SC_IntVector& aThreadCounts);
	virtual void DispatchRays(uint32 aThreadCountX, uint32 aThreadCountY = 1, uint32 aThreadCountZ = 1);
	virtual SC_Ref<SR_BufferResource> CreateAccelerationStructure(const SR_AccelerationStructureInputs& aInputs, SR_BufferResource* aExistingBufferResource);
	SC_Ref<SR_Buffer> BuildRaytracingBuffer(const SC_Array<SR_RaytracingInstanceData>& aInstances, SR_Buffer* aExistingBuffer);
#endif

	// Clears
	virtual void ClearRenderTarget(SR_RenderTarget* aRTV, const SC_Vector4& aClearColor);
	void ClearRenderTargets(uint32 aNumRTVs, SR_RenderTarget** aRTVs, const SC_Vector4& aClearColor = SC_Vector4(0.f));
	virtual void ClearDepthStencil(SR_DepthStencil* aDSV, float aClearValue = 0.f, uint8 aStencilClearValue = 0, SR_DepthClearFlags aClearFlags = SR_DepthClearFlags::All);

	// Bindings
	virtual  void SetShaderState(SR_ShaderState* aShaderState);

	virtual void SetRootSignature(SR_RootSignature* aRootSignature);

	virtual void SetVertexBuffer(SR_BufferResource* aVertexBuffer);
	virtual void SetIndexBuffer(SR_BufferResource* aIndexBuffer);

	virtual void SetPrimitiveTopology(const SR_PrimitiveTopology& aPrimitiveTopology);

	virtual void SetRootConstant();
	virtual void SetRootConstantBuffer(SR_BufferResource* aConstantBuffer, uint32 aSlot);
	virtual void SetRootConstantBuffer(SR_BufferResource* aConstantBuffer, uint64 aBufferOffset, uint32 aSlot);
	virtual void SetResourceInfo(uint8* aData, uint32 aSize);
	virtual void SetRootShaderResource();
	virtual void SetRootUnorderedAccessResource();

	virtual void SetTexture(SR_Texture* aTexture, uint32 aSlot);
	virtual void SetBuffer(SR_Buffer* aBuffer, uint32 aSlot);

	void SetRenderTarget(SR_RenderTarget* aRTV, SR_DepthStencil* aDSV);
	virtual void SetRenderTargets(uint32 aNumRTVs, SR_RenderTarget** aRTVs, SR_DepthStencil* aDSV);

	virtual void SetBlendFactor(const SC_Vector4& aBlendFactor);

	// Barriers
	void TransitionBarrier(uint32 aTargetResourceState, SR_Resource* aResource);
	virtual void TransitionBarrier(const SC_Array<SC_Pair<uint32, SR_Resource*>>& aTransitions);
	virtual void UnorderedAccessBarrier(SR_Resource* aResource);
	virtual void AliasBarrier();

	// Copy
	virtual void CopyResource(SR_Resource* aDstResource, SR_Resource* aSrcResource);
	virtual void CopyBuffer(SR_BufferResource* aDstBuffer, uint64 aDstOffset, SR_BufferResource* aSrcBuffer, uint64 aSrcOffset, uint32 aSize);
	virtual void CopyTexture(SR_TextureResource* aDstTexture, SR_TextureResource* aSrcResource);

	virtual void UpdateBuffer(SR_BufferResource* aDstBuffer, uint32 aDstOffset, const void* aData, uint32 aSize);

	virtual void UpdateTexture(SR_TextureResource* aTextureResource, const SR_PixelData* aData, uint32 aDataCount, bool aKeepData);
	virtual void UpdateTexture(SR_TextureResource* aTextureResource, const SC_IntVector& aDstOffset, const SC_IntVector& aSize, const SR_PixelData& aData, bool aKeepData);

	// Temp Resources

	virtual SR_BufferResource* GetBufferResource(uint64& aOutOffset, SR_BufferBindFlag aBufferType, uint32 aByteSize, void* aInitialData, uint32 aAlignment = 0, const SR_Fence& aCompletionFence = SR_Fence());
	virtual SR_Buffer* GetBuffer();

	// Misc
	virtual void SetViewport(const SR_Rect& aRect, float aMinDepth = 0.0f, float aMaxDepth = 1.0f);
	virtual void SetScissorRect(const SR_Rect& aRect);

	void WaitFor(SR_TaskEvent* aEvent);
	void WaitFor(const SC_UniquePtr<SR_TaskEvent>& aEvent);
	const SC_Array<SR_Fence>& GetFenceWaits() const;

	const SR_CommandListType& GetType() const;
protected:

	bool InitRingBuffers();

	struct ResourceBindings
	{
		ResourceBindings() { Clear(); }
		void Clear()
		{
			SC_Fill(mConstantBuffers, 2, nullptr);
			SC_Fill(mConstantBufferOffsets, 2, 0);
			mConstantsDirty.reset();

			SC_Fill(mTextures, 1, nullptr);
			mTexturesDirty.reset();
		}

		SR_BufferResource* mConstantBuffers[2];
		uint64 mConstantBufferOffsets[2];
		std::bitset<4> mConstantsDirty;

		SR_Texture* mTextures[1];
		std::bitset<1> mTexturesDirty;

	} mResourceBindings;
	SC_Array<SC_Ref<SR_Resource>> mTempResources;
	SC_Array<SR_Fence> mFenceWaits;

	SR_RingBuffer mVertexIndexRingBuffer;
	SR_RingBuffer mConstantsRingBuffer;
	SR_RingBuffer mBuffersRingBuffer;
	SR_RingBuffer mStagingRingBuffer;

	SR_CommandListType mType;
};

