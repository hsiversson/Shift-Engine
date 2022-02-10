#include "SGfx_Sky.h"
#include "View/SGfx_View.h"

SGfx_Sky::SGfx_Sky()
{

}

SGfx_Sky::~SGfx_Sky()
{

}

bool SGfx_Sky::Init()
{
	SR_TextureResourceProperties skyCubeResourceProps;
	skyCubeResourceProps.mSize = SC_IntVector(512, 512, 0);
	skyCubeResourceProps.mIsCubeMap = true;
	skyCubeResourceProps.mAllowRenderTarget = true;
	skyCubeResourceProps.mFormat = SR_Format::RG11B10_FLOAT;
	skyCubeResourceProps.mNumMips = 1;
	skyCubeResourceProps.mType = SR_ResourceType::Texture2D;
	skyCubeResourceProps.mDebugName = "CapturedSkyCube";
	SC_Ref<SR_TextureResource> cubeResource = SR_RenderDevice::gInstance->CreateTextureResource(skyCubeResourceProps);

	SR_TextureProperties skyCubeProps;
	skyCubeProps.mFormat = SR_Format::RG11B10_FLOAT;
	skyCubeProps.mDimension = SR_TextureDimension::TextureCube;
	mCapturedSkyCube = SR_RenderDevice::gInstance->CreateTexture(skyCubeProps, cubeResource);

	return true;
}

void SGfx_Sky::Render(SGfx_View* /*aView*/)
{
	//const SGfx_ViewData& renderData = aView->GetRenderData();


}

void SGfx_Sky::RenderSkyCube(SGfx_View* aView)
{
	const SGfx_ViewData& renderData = aView->GetRenderData();

	SGfx_Camera camera;
	camera.SetPerspectiveProjection({ 512.f, 512.f }, 1.f, 100.f, 90.f);
	camera.SetPosition(SC_Vector(0.f));

	const SC_Vector targets[6] =
	{
		SC_Vector(1, 0, 0),
		SC_Vector(-1, 0, 0),
		SC_Vector(0, 1, 0),
		SC_Vector(0, -1, 0),
		SC_Vector(0, 0, 1),
		SC_Vector(0, 0, -1),
	};

	for (uint32 i = 0; i < 6; ++i)
	{
		SC_Vector up = { 0.0f, 1.0f, 0.0f };
		if (i == 2)
			up = { 0.0f, 0.0f, -1.0f };
		else if (i == 3)
			up = { 0.0f, 0.0f, 1.0f };

		camera.LookAt(targets[i], up);
		RenderProbeFace(i, camera, renderData);
	}
}

void SGfx_Sky::RenderProbeFace(const uint32 /*aFace*/, const SGfx_Camera& /*aFaceCamera*/, const SGfx_ViewData& /*aRenderData*/)
{
}
