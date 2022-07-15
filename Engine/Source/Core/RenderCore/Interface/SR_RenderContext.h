#pragma once

enum class SR_RenderContextType
{
	Unknown,
	Graphics,
	Compute,
	Copy,

	COUNT_PUBLIC,

	Init_Quick = COUNT_PUBLIC,
	Init_Slow,

	COUNT
};

class SR_RenderContext
{
public:
	SR_RenderContext(const SR_RenderContextType& aContextType);
	virtual ~SR_RenderContext();

	virtual void Begin() = 0;
	virtual void End() = 0;

	virtual void Present() = 0;

	// Draw
	virtual void Draw(uint32 aVertexCount, uint32 aStartVertex = 0) = 0;
	virtual void DrawInstanced(uint32 aVertexCount, uint32 aStartVertex = 0) = 0;
	virtual void DrawIndirect(SR_Buffer* aArgs, uint32 aByteOffset = 0) = 0;
	virtual void DrawInstancedIndirect(SR_Buffer* aArgs, uint32 aByteOffset = 0) = 0;

	virtual void DrawIndexed(uint32 aIndexCount, uint32 aStartIndex = 0, uint32 aStartVertex = 0) = 0;
	virtual void DrawIndexedInstanced(uint32 aIndexCount, uint32 aStartIndex = 0, uint32 aStartVertex = 0) = 0;
	virtual void DrawIndexedIndirect(SR_Buffer* aArgs, uint32 aByteOffset = 0) = 0;
	virtual void DrawIndexedInstancedIndirect(SR_Buffer* aArgs, uint32 aByteOffset = 0) = 0;

	// Dispatch
	virtual void Dispatch(uint32 aThreadGroupX, uint32 aThreadGroupY = 1, uint32 aThreadGroupZ = 1) = 0;
	void Dispatch(const SC_IntVector3& aThreadGroups);
	void Dispatch(SR_ShaderState* aShader, uint32 aThreadGroupX, uint32 aThreadGroupY = 1, uint32 aThreadGroupZ = 1);
	void Dispatch(SR_ShaderState* aShader, const SC_IntVector3& aThreadGroups);

	virtual void DispatchIndirect(SR_Buffer* aArgs, uint32 aByteOffset = 0) = 0;

	// States
	virtual void SetShaderState(SR_ShaderState* aShaderState) = 0;

	// Bindings
	virtual void BindTexture();
	virtual void BindRWTexture();

	virtual void BindBuffer();
	virtual void BindRWBuffer();

	virtual void BindSampler();

protected:

	const SR_RenderContextType mContextType;

	static thread_local SC_Ref<SR_CommandList> mCurrentCommandList;

	static SR_RenderContext* gContextInstance;
};

