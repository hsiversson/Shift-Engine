#pragma once
#include "SED_Window.h"

class SED_MetricsWindow : public SED_Window
{
public:
	SED_MetricsWindow();
	~SED_MetricsWindow();

	const char* GetWindowName() const override { return "Metrics"; }

protected:
	void OnUpdate() override;
	void OnDraw() override;

private:
	uint32 mFPS;
	float mDeltaTimeCPU;

	SC_SizeT mAvailableSystemMemoryMB;
	SC_SizeT mUsedSystemMemoryMB;
	SC_SizeT mAvailableVideoMemoryMB;
	SC_SizeT mUsedVideoMemoryMB;
};

