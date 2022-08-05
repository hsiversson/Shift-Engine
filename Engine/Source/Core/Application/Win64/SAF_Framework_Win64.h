#pragma once
#include "Application/SAF_Framework.h"


#if IS_WINDOWS_PLATFORM

class SC_MessageQueue;
class SAF_Framework_Win64 : public SAF_Framework
{
public:
	SAF_Framework_Win64();
	~SAF_Framework_Win64();

	void* GetNativeWindowHandle() const override;
	float GetWindowDPI() const override;

protected:
	bool Init() override;
	bool Update() override;
	void Exit() override;

private:
	bool CreateConsole();

	void SyncWindowState();

	SC_MessageQueue* mMessageQueue;
};

#endif