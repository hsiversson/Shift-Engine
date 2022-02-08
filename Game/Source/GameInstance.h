#pragma once
#include "SGF_Framework.h"

class GameInstance : public SGF_GameInterface
{
public:
	GameInstance();
	~GameInstance();

	bool Init() override;
	void Update() override;
	void Exit() override;
};

