#include "SGF_Framework.h"

SGF_Framework::SGF_Framework()
	: mGameInstance(nullptr)
{

}

SGF_Framework::~SGF_Framework()
{

}

bool SGF_Framework::Init(SGF_GameInterface* aGameInstance)
{
	mGameInstance = aGameInstance;

	// Init engine modules
	// Create window
	// Init game modules

	return true;
}

void SGF_Framework::Run()
{
	while (UpdateSingleFrame()) {}
}

void SGF_Framework::Exit()
{

}

bool SGF_Framework::UpdateSingleFrame()
{

	mGameInstance->Update();

	return true;
}
