#pragma once
#include "GameFramework/Entity/SGF_Entity.h"

class SGF_World;

class SGF_Level : public SC_ReferenceCounted
{
	friend class SED_WorldHierarchyWindow;
public:
	SGF_Level();
	~SGF_Level();

	void Update();

	bool Load(const SC_FilePath& aFilePath);
	bool Save(const SC_FilePath& aFilePath);

	void AddEntity(const SGF_Entity& aEntity);

	SGF_Entity* FindEntityWithId(const SC_UUID& aId) const;

	void SetWorld(SGF_World* aWorld);
	SGF_World* GetWorld() const;

	void SetVisible(bool aValue);
	bool IsVisible() const;

private:
	SC_Mutex mEntitiesMutex;
	SC_Array<SGF_Entity> mEntities;

	SGF_World* mParentWorld;
	bool mIsVisible;
};