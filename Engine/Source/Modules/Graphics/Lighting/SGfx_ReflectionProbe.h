#pragma once

struct SGfx_ViewData;
class SGfx_Camera;

class SGfx_ReflectionProbe : public SC_ReferenceCounted
{
public:
	SGfx_ReflectionProbe();
	~SGfx_ReflectionProbe();

	void Render(SR_CommandList* aCmdList, const SGfx_ViewData& aRenderData);

	SR_Texture* GetTexture() const;

private:
	bool CreateResources();

	void RenderFace(SR_CommandList* aCmdList, uint32 aFaceIndex, const SGfx_Camera& aCamera, const SGfx_ViewData& aRenderData);

	SC_Vector mPosition;
	SC_IntVector2 mResolution;
	float mDistance;

	SC_Ref<SR_RenderTarget> mProbeRTs[6];
	SC_Ref<SR_Texture> mProbeCubemap;
	SC_Ref<SR_DepthStencil> mDepthStencil;
	SC_Ref<SR_BufferResource> mSceneConstantsBuffer[6];
	SC_Array<SC_Ref<SR_BufferResource>> mDrawInfoBuffers[6];
};