#pragma once
#include "RenderCore/Interface/SR_CommandList.h"

#if SR_ENABLE_DX12

struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12GraphicsCommandList6;
class SR_RootSignature_DX12;
class SR_ShaderState_DX12;
class SR_TextureResource_DX12;

class SR_CommandList_DX12 : public SR_CommandList
{
public:
	SR_CommandList_DX12(const SR_CommandListType& aType);
	~SR_CommandList_DX12();

	bool Init(const char* aDebugName = nullptr);

	void Begin() override;
	void End() override;

	void BeginEvent(const char* aName) override;
	void EndEvent() override;

	// Draw/Dispatch
	void Dispatch(uint32 aGroupCountX, uint32 aGroupCountY = 1, uint32 aGroupCountZ = 1) override;

	void DrawInstanced(uint32 aVertexCount, uint32 aInstanceCount, uint32 aStartVertex = 0, uint32 aStartInstance = 0) override;
	void DrawIndexedInstanced(uint32 aIndexCount, uint32 aInstanceCount, uint32 aStartIndex = 0, uint32 aStartVertex = 0, uint32 aStartInstance = 0) override;

#if SR_ENABLE_MESH_SHADERS
	void DispatchMesh(uint32 aGroupCountX, uint32 aGroupCountY = 1, uint32 aGroupCountZ = 1) override;
#endif

#if SR_ENABLE_RAYTRACING
	void DispatchRays(uint32 aThreadCountX, uint32 aThreadCountY = 1, uint32 aThreadCountZ = 1) override;

	SC_Ref<SR_BufferResource> CreateAccelerationStructure(const SR_AccelerationStructureInputs& aInputs, SR_BufferResource* aExistingBufferResource) override;
#endif

	// Clears
	void ClearRenderTarget(SR_RenderTarget* aRTV, const SC_Vector4& aClearColor) override;
	void ClearDepthStencil(SR_DepthStencil* aDSV, float aClearValue = 0.f, uint8 aStencilClearValue = 0, SR_DepthClearFlags aClearFlags = SR_DepthClearFlags::All) override;

	// Bindings
	void SetShaderState(SR_ShaderState* aShaderState) override;

	void SetRootSignature(SR_RootSignature* aRootSignature) override;

	void SetVertexBuffer(SR_BufferResource* aVertexBuffer) override;
	void SetIndexBuffer(SR_BufferResource* aIndexBuffer) override;

	void SetPrimitiveTopology(const SR_PrimitiveTopology& aPrimitiveTopology) override;

	void SetRootConstant() override;
	void SetRootConstantBuffer(SR_BufferResource* aConstantBuffer, uint32 aSlot) override;
	void SetResourceInfo(uint8* aData, uint32 aSize) override;
	void SetRootShaderResource() override;
	void SetRootUnorderedAccessResource() override;

	void SetTexture(SR_Texture* aTexture, uint32 aSlot) override;
	void SetBuffer(SR_Buffer* aBuffer, uint32 aSlot) override;

	void SetRenderTargets(uint32 aNumRTVs, SR_RenderTarget** aRTVs, SR_DepthStencil* aDSV) override;

	void SetBlendFactor(const SC_Vector4& aBlendFactor) override;

	// Barriers
	void TransitionBarrier(const SC_Array<SC_Pair<uint32, SR_Resource*>>& aTransitions) override;
	void UnorderedAccessBarrier(SR_Resource* aResource) override;
	void AliasBarrier();

	// Copy
	void CopyResource(SR_Resource* aDstResource, SR_Resource* aSrcResource) override;
	void CopyBuffer(SR_BufferResource* aDstBuffer, uint32 aDstOffset, SR_BufferResource* aSrcBuffer, uint32 aSrcOffset, uint32 aSize) override;
	void CopyTexture(SR_TextureResource* aDstTexture, SR_TextureResource* aSrcResource) override;

	void UpdateBuffer(SR_BufferResource* aDstBuffer, uint32 aDstOffset, const void* aData, uint32 aSize) override;
	void UpdateTexture(SR_TextureResource* aTextureResource, const SR_PixelData* aData, uint32 aDataCount, bool aKeepData) override;
	void UpdateTexture(SR_TextureResource* aTextureResource, const SC_IntVector& aDstOffset, const SC_IntVector& aSize, const SR_PixelData& aData, bool aKeepData) override;

	// Misc
	void SetViewport(const SR_Rect& aRect, float aMinDepth = 0.0f, float aMaxDepth = 1.0f) override;
	void SetScissorRect(const SR_Rect& aRect) override;

	ID3D12GraphicsCommandList* GetD3D12CommandList() const;

private:

	void UpdateTextureInternal(SR_TextureResource_DX12* aTextureResource, const SC_IntVector& aDstOffset, const SC_IntVector& aSize, const SR_TextureResourceProperties& aCopyTextureProperties, const SR_PixelData* aData, uint32 aDataCount, uint32 aFirstSubresource);

	void SetResources();

	struct RenderStateCache
	{
		RenderStateCache() { Clear(); }
		void Clear()
		{
			mCurrentShaderState = nullptr;
			mCurrentRootSignature = nullptr;
			mRasterizerState = nullptr;
			mBlendState = nullptr;
			mDepthStencilState = nullptr;
		}

		SR_ShaderState_DX12* mCurrentShaderState;
		SR_RootSignature_DX12* mCurrentRootSignature;

		SR_RasterizerState* mRasterizerState;
		SR_BlendState* mBlendState;
		SR_DepthStencilState* mDepthStencilState;
	} mStateCache;

	SR_ComPtr<ID3D12CommandAllocator> mD3D12CommandAllocator;
	SR_ComPtr<ID3D12GraphicsCommandList> mD3D12CommandList;
	SR_ComPtr<ID3D12GraphicsCommandList6> mD3D12CommandList6;

	SC_Ref<SR_BufferResource> mAccelerationStructureScratchBuffer;

#if SR_ENABLE_NVIDIA_AFTERMATH
	void* mNvAftermathContextHandle;
#endif
};

#endif