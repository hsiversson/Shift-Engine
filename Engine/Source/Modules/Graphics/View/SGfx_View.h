#pragma once
#include "SGfx_ViewData.h"
#include "SGfx_Camera.h"

class SR_Texture;

class SGfx_View
{
public:
	SGfx_View();
	~SGfx_View();

	SGfx_ViewData& GetPrepareData(); // Should only be accessed from prepare tasks
	const SGfx_ViewData& GetRenderData(); // Should only be accessed from render tasks
	SGfx_ViewData& GetMutableRenderData(); // Should only be accessed from render tasks

	void SetCamera(const SGfx_Camera& aCamera);
	const SGfx_Camera& GetCamera() const;

	void SetOnlyDepth(bool aValue);
	bool IsOnlyDepth() const;
	void SetMainView(bool aValue);
	bool IsMainView() const;

	void SetOutputTexture(const SC_Ref<SR_Texture>& aTexture);
	SR_Texture* GetOutputTexture() const;

	void EndRender(); // Called once all render tasks have finished recording on the CPU side.

private:
	SC_Ref<SR_Texture> mOutputTexture;
	SGfx_ViewData mRenderData;
	SGfx_Camera mCamera;
	bool mOnlyDepth;
	bool mIsMainView;
};

