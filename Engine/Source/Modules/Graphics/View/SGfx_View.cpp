#include "SGfx_View.h"

SGfx_View::SGfx_View()
    : mOnlyDepth(false)
    , mIsMainView(false)
{
}

SGfx_View::~SGfx_View()
{
}

SGfx_ViewData& SGfx_View::GetPrepareData()
{
    return mRenderData;
}

const SGfx_ViewData& SGfx_View::GetRenderData()
{
	return mRenderData;
}

SGfx_ViewData& SGfx_View::GetMutableRenderData()
{
	return mRenderData;
}

void SGfx_View::SetCamera(const SGfx_Camera& aCamera)
{
    mCamera = aCamera;
}

const SGfx_Camera& SGfx_View::GetCamera() const
{
    return mCamera;
}

void SGfx_View::SetOnlyDepth(bool aValue)
{
    mOnlyDepth = aValue;
}

bool SGfx_View::IsOnlyDepth() const
{
    return mOnlyDepth;
}

void SGfx_View::SetMainView(bool aValue)
{
    mIsMainView = aValue;
}

bool SGfx_View::IsMainView() const
{
    return mIsMainView;
}

void SGfx_View::SetOutputTexture(const SC_Ref<SR_Texture>& aTexture)
{
    mOutputTexture = aTexture;
}

SR_Texture* SGfx_View::GetOutputTexture() const
{
    return mOutputTexture.get();
}
