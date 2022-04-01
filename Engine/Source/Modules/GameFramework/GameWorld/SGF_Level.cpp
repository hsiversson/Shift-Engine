#include "SGF_Level.h"
#include "GameFramework/Entity/Components/SGF_EntityIdComponent.h"

SGF_Level::SGF_Level()
    : mParentWorld(nullptr)
{

}

SGF_Level::~SGF_Level()
{

}

void SGF_Level::Update()
{
    for (SC_Ref<SGF_Entity>& entity : mEntities)
    { 
        entity->Update();
    }
}

bool SGF_Level::Load(const SC_FilePath& aFilePath)
{
    SC_Timer timer;

    SC_Json loadData;
    if (!SC_LoadJson(aFilePath, loadData))
        return false;

    mEntities.Reserve((uint32)loadData["Entities"].size());
    SC_Array<SC_Future<bool>> futures;
    futures.Reserve((uint32)loadData["Entities"].size());
    for (const SC_Json& entityData : loadData["Entities"])
    {
        auto CreateEntity = [this, entityData]()
        {
            SC_Ref<SGF_Entity> entity = SC_MakeRef<SGF_Entity>();
            entity->SetWorld(mParentWorld);
            entity->AddComponent<SGF_EntityIdComponent>();
            if (!entity->Load(entityData))
                return; // display error msg

            AddEntity(entity);
        };
        futures.Add(SC_ThreadPool::Get().SubmitTask(CreateEntity));
    }

    for (const SC_Future<bool>& future : futures)
    {
        future.Wait();
    }

    SC_LOG("Loading level [%s] took: %.3f ms", SC_FilePath::GetFileNameWithoutExtension(aFilePath).c_str(), timer.Stop<float>());

    return true;
}

bool SGF_Level::Save(const SC_FilePath& aFilePath)
{
    SC_Json saveData;

    for (const SC_Ref<SGF_Entity>& entity : mEntities)
	{
		SC_Json entityData;
        entity->Save(entityData);
        saveData["Entities"].push_back(entityData);
    }

    return SC_SaveJson(aFilePath, saveData);
}

void SGF_Level::AddEntity(SC_Ref<SGF_Entity> aEntity)
{
    SC_MutexLock lock(mEntitiesMutex);
    mEntities.Add(aEntity);
}

SGF_Entity* SGF_Level::FindEntityWithId(const SC_UUID& aId) const
{
    for (const SC_Ref<SGF_Entity>& entity : mEntities)
    {
        if (entity->Is(aId))
            return entity.get();
    }

    return nullptr;
}

void SGF_Level::SetWorld(SGF_World* aWorld)
{
    mParentWorld = aWorld;
}

SGF_World* SGF_Level::GetWorld() const
{
    return mParentWorld;
}
