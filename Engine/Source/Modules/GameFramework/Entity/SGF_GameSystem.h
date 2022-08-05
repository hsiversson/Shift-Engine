#pragma once

class SGF_EntityManager;
class SGF_ComponentManager;

class SGF_GameSystem : public SC_ReferenceCounted
{
	friend class SGF_GameSystemManager;
public:
	SGF_GameSystem()
		: mEntityManager(nullptr)
		, mComponentManager(nullptr)
	{}
	virtual ~SGF_GameSystem() {}

	virtual bool Init() { return true; }
	virtual void Update() {}

protected:
	SGF_EntityManager* mEntityManager;
	SGF_ComponentManager* mComponentManager;
};

class SGF_GameSystemManager
{
public:
	SGF_GameSystemManager(SGF_EntityManager* aEntityManager, SGF_ComponentManager* aComponentManager);
	~SGF_GameSystemManager();

	bool Init();
	void Update();

	void AddSystem(const SC_Ref<SGF_GameSystem>& aSystem);
	void RemoveSystem(const SC_Ref<SGF_GameSystem>& aSystem);

private:
	SC_Array<SC_Ref<SGF_GameSystem>> mGameSystems;

	SGF_EntityManager* mEntityManager;
	SGF_ComponentManager* mComponentManager;
};