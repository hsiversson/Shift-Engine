#pragma once

struct SGfx_LightCullingConstants
{
	SGfx_LightCullingConstants()
		: mTotalNumLights(0)
		, mTileGridDescriptorIndex(0)
		, mLightBufferDescriptorIndex(0)
		, _unused0{}
	{}

	SC_Vector2u mNumTiles;
	uint32 mTotalNumLights;

	uint32 mTileGridDescriptorIndex;
	uint32 mLightBufferDescriptorIndex;
	uint32 _unused0[3];
};

struct SGfx_ViewData;
class SR_Buffer;
class SR_CommandList;
class SR_BufferResource;
class SR_ShaderState;
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
	struct LightTile
	{
		static constexpr uint32 gTileSize = 32;
		static constexpr uint32 gNumLightsPerTile = 256;

		uint32 mLightIndices[gNumLightsPerTile];
		uint32 mNumLights;
	};
	struct LightTileFrustum
	{
		SC_Plane mPlanes[4]; // Left, Top, Right, Bottom (Near & Far is defined by camera)
	};
	SC_Array<LightTile> mLightTiles;
	SC_Array<LightTileFrustum> mLightTileFrustums;

	void CullLightsCPU();

private:
	static constexpr uint32 gTileSize = 16;
	static constexpr uint32 gNumLightsPerTile = 63;

	SGfx_LightCullingConstants mConstants;

	SC_Array<SC_Ref<SR_Buffer>> mDelayedDeleteResources;

	SC_Ref<SR_BufferResource> mCullConstants;
	SC_Ref<SR_ShaderState> mLightCullingShader;

	SC_Ref<SR_Buffer> mLightBuffer;
	SC_Ref<SR_Buffer> mTileGridBuffer;
	SC_Ref<SR_Buffer> mTileGridBufferRW;
};

