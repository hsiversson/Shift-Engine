#pragma once

struct SGfx_ViewData;
class SGfx_AmbientOcclusion
{
public:
	enum Type
	{
		None,
		GTAO,
		RTAO,
	};

	struct GTAOSettings
	{
		GTAOSettings() {}
	};

	struct RTAOSettings
	{
		RTAOSettings() : mNumRaysPerPixel(2), mRadius(1.0f), mUseDenoiser(true), mAOType(Type::RTAO) {}

		int32 mNumRaysPerPixel;
		float mRadius;
		bool mUseDenoiser;
		Type mAOType;
	};

public:
	SGfx_AmbientOcclusion();
	~SGfx_AmbientOcclusion();

	bool Init();

	void Render(SR_CommandList* aCmdList, const SC_Ref<SR_Texture>& aDepthBuffer, const SGfx_ViewData& aRenderData);

	SR_Texture* GetTexture() const;

	const RTAOSettings& GetRTAOSettings() const;
	RTAOSettings& GetRTAOSettings();

private:
	void RenderGTAO(SR_CommandList* aCmdList, const SC_Ref<SR_Texture>& aDepthBuffer, const SGfx_ViewData& aRenderData);
#if SR_ENABLE_RAYTRACING
	void RenderRTAO(SR_CommandList* aCmdList, const SC_Ref<SR_Texture>& aDepthBuffer, const SGfx_ViewData& aRenderData);
#endif

	SC_Ref<SR_Texture> mRawOutputRWTexture;
	SC_Ref<SR_Texture> mRawOutputTexture;
	SC_Ref<SR_Texture> mDenoisedRWTexture[2];
	SC_Ref<SR_Texture> mDenoisedTexture[2];
	SC_Ref<SR_BufferResource> mConstantBuffer;
	SC_Ref<SR_BufferResource> mDenoiserConstantBuffer[2];

	//SC_Ref<SR_ShaderState> mPrepassShader;
	//SC_Ref<SR_ShaderState> mGTAOShader;
	//SC_Ref<SR_ShaderState> mDenoiseShader;

	SC_Ref<SR_ShaderState> mRTAOShader;
	SC_Ref<SR_ShaderState> mDenoiseShader;
	RTAOSettings mRTAOSettings;

};

