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

	bool Init(SGF_GameInterface* aGameInstance);
	void Run();
	void Exit();

private:
	bool UpdateSingleFrame();

private:
	SGF_GameInterface* mGameInstance;
};

