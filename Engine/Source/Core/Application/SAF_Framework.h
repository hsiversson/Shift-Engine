#pragma once

class SAF_AppCallbacks
{
public:
	virtual ~SAF_AppCallbacks() {}
	virtual bool Init() { return true; }
	virtual bool Update() { return true; }
	virtual bool Render() { return true; }
	virtual void Exit() {}
};

struct SAF_FrameworkCreateParams
{
	SAF_FrameworkCreateParams() : mCallbacks(nullptr) {}

	SAF_AppCallbacks* mCallbacks;
};

class SAF_Framework
{
public:
	SAF_Framework();
	virtual ~SAF_Framework();

	int AppMain();
	
	virtual void* GetNativeWindowHandle() const;
	virtual float GetWindowDPI() const;

	void SetFullscreen(bool aValue);

	static SAF_Framework* Create(const SAF_FrameworkCreateParams& aCreateParams);
	static SAF_Framework* Get();
	static void Destroy();
protected:
	virtual bool Init();
	virtual bool Update();
	virtual void Exit();

	void RequestExit();

	SAF_AppCallbacks* mCallbacks;

	bool mIsFullscreen;
private:

	void MainLoop();

	bool mExitRequested;

	static SAF_Framework* gInstance;
};

