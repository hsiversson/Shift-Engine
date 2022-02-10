#pragma once
#include "Application/SAF_Framework.h"

class GameInstance : public SAF_AppCallbacks
{
public:
	GameInstance();
	~GameInstance();

	virtual bool Init() override;
	virtual bool Update() override;
	virtual bool Render() override;
	virtual void Exit() override;
};

