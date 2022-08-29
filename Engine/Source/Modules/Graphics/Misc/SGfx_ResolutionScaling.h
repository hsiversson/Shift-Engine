#pragma once

enum class SGfx_ResolutionScalingType
{
	TAA,
	DLSS,
	FSR2,

	Default = FSR2,
};

class SGfx_ResolutionScaling
{
public:
	SGfx_ResolutionScaling();
	~SGfx_ResolutionScaling();

	bool Init();

	void SetType(const SGfx_ResolutionScalingType& aScalingType);
	const SGfx_ResolutionScalingType& GetType() const;

private:

	SGfx_ResolutionScalingType mType;
};

