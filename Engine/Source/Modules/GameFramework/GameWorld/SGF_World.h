#pragma once
#include "SGF_Level.h"

/*
* 
*	Acts as a container for several levels which holds most of the world objects
*	Also holds special-case objects that for example can move in between levels (eg. the player)
*	Can seamlessly stream levels in and out
* 
*/

class SGfx_World;
class SGF_EntityManager;
class SGF_GameSystemManager;
class SGF_RenderSystem;
class SGF_World : public SC_ReferenceCounted
{
	friend class SED_WorldHierarchyWindow;
public:
	SGF_World();
	~SGF_World();

	bool Init();

	bool LoadLevel(const char* aLevel);

	void Update();

	void AddLevel(const SC_Ref<SGF_Level>& aLevel);
	void RemoveLevel();

	SGF_Entity* FindEntityWithId(const SC_UUID& aId) const;

	SGfx_World* GetGraphicsWorld() const;
	SGF_EntityManager* GetEntityManager() const;
	SGF_GameSystemManager* GetGameSystemManager() const;

private:
	void RegisterComponents();

	SC_Array<SC_Ref<SGF_Level>> mLevels;

	SC_UniquePtr<SGF_ComponentManager> mComponentManager;
	SC_UniquePtr<SGF_EntityManager> mEntityManager;
	SC_UniquePtr<SGF_GameSystemManager> mGameSystemManager;
	SC_Ref<SGF_RenderSystem> mRenderSystem;
};