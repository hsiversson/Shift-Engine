#pragma once


struct SGfx_LightCullingConstants
{
	SC_Vector2u mNumTiles;
	uint32 mTotalNumLights;

	uint32 mTileGridDescriptorIndex;
	uint32 mLightBufferDescriptorIndex;
	uint32 _unused0[3];
};

class SR_Buffer;
struct SGfx_ViewData;
class SGfx_LightCulling
{
public:
	SGfx_LightCulling();
	~SGfx_LightCulling();

	bool Init();

	void Prepare(SGfx_ViewData& aPrepareData);

	void CullLights(SR_CommandList* aCmdList, const SGfx_ViewData& aRenderData, const SR_Texture* aDepthBuffer);

	const SGfx_LightCullingConstants& GetConstants() const;

	SR_Buffer* GetLightBuffer() const;
	SR_Buffer* GetTileGridBuffer() const;

private:
	static constexpr uint32 gTileSize = 16;
	static constexpr uint32 gNumLightsPerTile = 31;

	SGfx_LightCullingConstants mConstants;

	SC_Array<SC_Ref<SR_Buffer>> mDelayedDeleteResources;

	SC_Ref<SR_BufferResource> mCullConstants;
	SC_Ref<SR_ShaderState> mLightCullingShader;

	SC_Ref<SR_Buffer> mLightBuffer;
	SC_Ref<SR_Buffer> mTileGridBuffer;
	SC_Ref<SR_Buffer> mTileGridBufferRW;
};

