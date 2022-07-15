#include "SGF_Level.h"
#include "GameFramework/Entity/Components/SGF_EntityIdComponent.h"
#include "SGF_World.h"

SGF_Level::SGF_Level()
    : mParentWorld(nullptr)
    , mIsVisible(true)
{

}

SGF_Level::~SGF_Level()
{

}

void SGF_Level::Update()
{
	//for (SC_Ref<SGF_Entity>& entity : mEntities)
	//{ 
	//    entity->Update();
	//}
}

bool SGF_Level::Load(const SC_FilePath& aFilePath)
{
    SC_Timer timer;

    SC_Json loadData;
    if (!SC_LoadJson(aFilePath, loadData))
        return false;

    mEntities.Reserve((uint32)loadData["Entities"].size());

	SGF_EntityManager* entityManager = mParentWorld->GetEntityManager();

    for (const SC_Json& entityData : loadData["Entities"])
    {
        SGF_Entity entity = entityManager->CreateEntity();
        entity.AddComponent<SGF_EntityIdComponent>();

        for (const SC_Json& componentData : entityData["Components"])
        {
            SGF_Component* comp = entity.AddComponent(componentData["Name"].get<std::string>().c_str());
            comp->Load(componentData);
        }
        mEntities.Add(entity);
    }

    SC_LOG("Loading level [{}] took: {:.3f} ms", SC_FilePath::GetFileNameWithoutExtension(aFilePath).c_str(), timer.Stop<float>());

    return true;
}

bool SGF_Level::Save(const SC_FilePath& aFilePath)
{
    SC_Json saveData;

	//for (const SC_Ref<SGF_Entity>& entity : mEntities)
	//{
	//  SC_Json entityData;
	//  //entity->Save(entityData);
	//  saveData["Entities"].push_back(entityData);
	//}

    return SC_SaveJson(aFilePath, saveData);
}

void SGF_Level::AddEntity(const SGF_Entity& aEntity)
{
	//SC_MutexLock lock(mEntitiesMutex);
	mEntities.Add(aEntity);
}

SGF_Entity* SGF_Level::FindEntityWithId(const SC_UUID& /*aId*/) const
{
	//for (const SC_Ref<SGF_Entity>& entity : mEntities)
	//{
	//	//if (entity->Is(aId))
	//	//    return entity.get();
	//}

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

void SGF_Level::SetVisible(bool aValue)
{
    mIsVisible = aValue;
	//for (SC_Ref<SGF_Entity>& entity : mEntities)
	//    entity->SetVisible(aValue);
}

bool SGF_Level::IsVisible() const
{
    return mIsVisible;
}
