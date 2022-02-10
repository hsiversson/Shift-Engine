#pragma once

class SGF_GameInterface
{
public:
	virtual bool Init() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
};

class SGF_Framework
{
public:
	SGF_Framework();
	~SGF_Framework();

	int32 InternalMain(SGF_GameInterface* aGameInstance);

private:
	bool Init();

	void MainLoop();
	bool Tick();

	void Exit();

private:
	SGF_GameInterface* mGameInstance;
};

