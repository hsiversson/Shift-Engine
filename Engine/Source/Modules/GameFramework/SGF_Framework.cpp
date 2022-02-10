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

	return mGameInstance->Init();
}

void SGF_Framework::Run()
{
	while (Tick()) {}
}

void SGF_Framework::Exit()
{

}

bool SGF_Framework::Tick()
{

	mGameInstance->Update();

	return true;
}
