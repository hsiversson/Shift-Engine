#pragma once
class SGfx_Volumetrics
{
public:
	struct FogSettings
	{
		SC_IntVector mResolution;


		bool mEnabled;
	};
	struct CloudSettings
	{

		bool mEnabled;
	};

public:
	SGfx_Volumetrics();
	~SGfx_Volumetrics();

	bool Init();

	void ComputeVolumetricFog();
	void ComputeVolumetricClouds();

	void ApplyVolumetrics();
private:
	FogSettings mFogSettings;
	CloudSettings mCloudSettings;


	SC_Ref<SR_Texture> mVolumetricScattering;
	SC_Ref<SR_Texture> mVolumetricScatteringRW;

};

