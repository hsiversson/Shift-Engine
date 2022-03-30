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
class SGF_World
{
	friend class SED_WorldHierarchyPanel;
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

private:
	SC_Array<SC_Ref<SGF_Level>> mLevels;
	SC_UniquePtr<SGfx_World> mGraphicsWorld;
};