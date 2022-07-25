#include "SED_MetricsWindow.h"
#include "RenderCore/Interface/SR_RenderDevice.h"

SED_MetricsWindow::SED_MetricsWindow()
{

}

SED_MetricsWindow::~SED_MetricsWindow()
{

}

void SED_MetricsWindow::OnUpdate()
{
	SC_PROFILER_FUNCTION();
	mFPS = SC_Time::gFramerate;
	mDeltaTimeCPU = SC_Time::gDeltaTime * 1000.f;

	mUsedSystemMemoryMB = BYTE_TO_MB(SC_Platform::GetPhysicalMemoryUsed());
	mAvailableSystemMemoryMB = BYTE_TO_MB(SC_Platform::GetPhysicalMemoryAvailable());

	mUsedVideoMemoryMB = BYTE_TO_MB(SR_RenderDevice::gInstance->GetUsedVRAM());
	mAvailableVideoMemoryMB = BYTE_TO_MB(SR_RenderDevice::gInstance->GetAvailableVRAM());
}

void SED_MetricsWindow::OnDraw()
{
	SC_PROFILER_FUNCTION();
	SED_Text("Application: %.3f ms/frame (%i FPS)", mDeltaTimeCPU, mFPS);
	SED_Text("Memory: %i MB (%i MB Total)", mUsedSystemMemoryMB, mAvailableSystemMemoryMB);
	SED_Text("VRAM: %i MB (%i MB Total)", mUsedVideoMemoryMB, mAvailableVideoMemoryMB);
}
