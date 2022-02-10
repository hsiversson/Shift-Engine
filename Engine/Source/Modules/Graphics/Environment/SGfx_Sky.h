#pragma once

class SGfx_View;
class SGfx_Camera;
struct SGfx_ViewData;

class SGfx_Sky
{
public:
	SGfx_Sky();
	~SGfx_Sky();

	bool Init();

	void Render(SGfx_View* aView);
	void RenderSkyCube(SGfx_View* aView);
private:
	void RenderProbeFace(const uint32 aFace, const SGfx_Camera& aFaceCamera, const SGfx_ViewData& aRenderData);

	SC_Ref<SR_Texture> mCapturedSkyCube;
};

