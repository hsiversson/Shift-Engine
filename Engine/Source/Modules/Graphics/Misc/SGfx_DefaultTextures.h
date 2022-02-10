#pragma once

class SR_Texture;
class SGfx_DefaultTextures
{
public:
	SGfx_DefaultTextures();
	~SGfx_DefaultTextures();

	static bool Init();
	static void Destroy();
	static const SC_Ref<SR_Texture>& GetWhite1x1();
	static const SC_Ref<SR_Texture>& GetBlack1x1();
	static const SC_Ref<SR_Texture>& GetGrey1x1();
	static const SC_Ref<SR_Texture>& GetFlatNormalMap1x1();

private:
	static SGfx_DefaultTextures* gInstance;

	SC_Ref<SR_Texture> mWhite1x1;
	SC_Ref<SR_Texture> mBlack1x1;
	SC_Ref<SR_Texture> mGrey1x1;
	SC_Ref<SR_Texture> mFlatNormalMap1x1;
};

