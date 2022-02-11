#pragma once
#include "GameFramework/Entity/SGF_Entity.h"

class SGF_World;

class SGF_Level
{
	friend class SED_WorldHierarchyPanel;
public:
	SGF_Level();
	~SGF_Level();

	void Update();

	bool Load(const SC_FilePath& aFilePath);
	bool Save(const SC_FilePath& aFilePath);

	void AddEntity(SC_Ref<SGF_Entity> aEntity);

	void SetWorld(SGF_World* aWorld);
	SGF_World* GetWorld() const;
private:
	SC_Mutex mEntitiesMutex;
	SC_Array<SC_Ref<SGF_Entity>> mEntities;

	SGF_World* mParentWorld;
};