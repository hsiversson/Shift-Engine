#include "SGF_GameSystem.h"

SGF_GameSystemManager::SGF_GameSystemManager(SGF_EntityManager* aEntityManager, SGF_ComponentManager* aComponentManager)
	: mEntityManager(aEntityManager)
	, mComponentManager(aComponentManager)
{

}

SGF_GameSystemManager::~SGF_GameSystemManager()
{

}

bool SGF_GameSystemManager::Init()
{
	return true;
}

void SGF_GameSystemManager::Update()
{
	for (const SC_Ref<SGF_GameSystem>& system : mGameSystems)
	{
		system->Update();
	}
}

void SGF_GameSystemManager::AddSystem(const SC_Ref<SGF_GameSystem>& aSystem)
{
	SC_Ref<SGF_GameSystem>& system = mGameSystems.Add(aSystem);
	system->mEntityManager = mEntityManager;
	system->mComponentManager = mComponentManager;
}

void SGF_GameSystemManager::RemoveSystem(const SC_Ref<SGF_GameSystem>& aSystem)
{
	mGameSystems.Remove(aSystem);
}
