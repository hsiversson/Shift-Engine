#include "SGfx_ResolutionScaling.h"

SGfx_ResolutionScaling::SGfx_ResolutionScaling()
	: mType(SGfx_ResolutionScalingType::Default)
{

}

SGfx_ResolutionScaling::~SGfx_ResolutionScaling()
{

}

bool SGfx_ResolutionScaling::Init()
{
	return false;
}

void SGfx_ResolutionScaling::SetType(const SGfx_ResolutionScalingType& aScalingType)
{
	if (mType != aScalingType)
	{
		mType = aScalingType;
		Init();
	}
}

const SGfx_ResolutionScalingType& SGfx_ResolutionScaling::GetType() const
{
	return mType;
}
