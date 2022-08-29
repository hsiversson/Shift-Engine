#include "SGF_World.h"
#include "Graphics/World/SGfx_World.h"

#include "GameFramework/Entity/SGF_Entity.h"
#include "GameFramework/Entity/SGF_GameSystem.h"
#include "GameFramework/Entity/GameSystem/SGF_RenderSystem.h"

////////////////////////////////
// TEMP
#include "../Editor/SED_AssimpImporter.h"
////////////////////////////////

#include "../Entity/Components/SGF_EntityIdComponent.h"
#include "../Entity/Components/SGF_TransformComponent.h"
#include "../Entity/Components/SGF_PointLightComponent.h"
#include "../Entity/Components/SGF_StaticMeshComponent.h"
#include "../Entity/Components/SGF_SpotLightComponent.h"
#include "../Entity/Components/SGF_DirectionalLightComponent.h"
#include "../Entity/Components/SGF_CameraComponent.h"
#include "../Entity/Components/SGF_AtmosphereComponent.h"

SGF_World::SGF_World()
{

}

SGF_World::~SGF_World()
{

}

bool SGF_World::Init()
{
	mComponentManager = SC_MakeUnique<SGF_ComponentManager>();
	RegisterComponents();

	mEntityManager = SC_MakeUnique<SGF_EntityManager>(this, mComponentManager.get());
	mGameSystemManager = SC_MakeUnique<SGF_GameSystemManager>(mEntityManager.get(), mComponentManager.get());

	mRenderSystem = SC_MakeRef<SGF_RenderSystem>();
	mRenderSystem->Init();
	mGameSystemManager->AddSystem(mRenderSystem);

	return true;
}

bool SGF_World::LoadLevel(const char* /*aLevel*/)
{
	SC_Timer timer;
	SC_Ref<SGF_Level> level = SC_MakeRef<SGF_Level>();
	level->SetWorld(this);
	//if (level->Load(SC_EnginePaths::Get().GetGameDataDirectory() + "/Levels/Sponza.slvl"))
	//	mLevels.Add(level);w

	SED_AssimpScene scene;
	SED_AssimpImporter importer;
	//importer.ImportScene(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shapes/ShaderBall.fbx", scene);
	importer.ImportScene(SC_EnginePaths::Get().GetGameDataDirectory() + "/Models/SponzaPBR/sponzaPBR.obj", scene, 0.01f);
	//importer.ImportScene(SC_EnginePaths::Get().GetGameDataDirectory() + "/Models/SunTemple/SunTemple.fbx", scene);

	scene.ConvertToLevelAndSave(*level);
	//level->Save(SC_EnginePaths::Get().GetGameDataDirectory() + "/Levels/Sponza.slvl");
	mLevels.Add(level);


	SGF_EntityManager* entityManager = GetEntityManager();
	SGF_Entity entity = entityManager->CreateEntity();
	entity.AddComponent<SGF_EntityIdComponent>();
	entity.AddComponent<SGF_EntityNameComponent>()->mName = "Light";

	SGF_TransformComponent* transform = entity.AddComponent<SGF_TransformComponent>();
	transform->mPosition = SC_Vector(4.f, 2.0f, 0.0f);

	SGF_PointLightComponent* lightComp = entity.AddComponent<SGF_PointLightComponent>();
	lightComp->mColor = SC_Color(128, 128, 255, 255);
	lightComp->mRange = 10.0f;
	lightComp->mIntensity = 140.0f;
	lightComp->mSourceRadius = 0.06f;
	lightComp->OnCreate();
	lightComp->OnUpdate();

	SC_LOG("Loading level [<name>] took: {:.3f} ms", timer.Stop<float>());
	return true;
}

void SGF_World::Update()
{
	SC_PROFILER_FUNCTION();
	mGameSystemManager->Update();
}

void SGF_World::AddLevel(const SC_Ref<SGF_Level>& aLevel)
{
	mLevels.Add(aLevel);
}

void SGF_World::RemoveLevel()
{

}

SGF_Entity* SGF_World::FindEntityWithId(const SC_UUID& aId) const
{
	for (const SC_Ref<SGF_Level>& level : mLevels)
	{
		SGF_Entity* entity = level->FindEntityWithId(aId);
		if (entity)
			return entity;
	}

	return nullptr;
}

SGfx_World* SGF_World::GetGraphicsWorld() const
{
	return mRenderSystem->GetGraphicsWorld();
}

SGF_EntityManager* SGF_World::GetEntityManager() const
{
	return mEntityManager.get();
}

SGF_GameSystemManager* SGF_World::GetGameSystemManager() const
{
	return mGameSystemManager.get();
}

void SGF_World::RegisterComponents()
{
	mComponentManager->RegisterComponentType<SGF_EntityIdComponent>();
	mComponentManager->RegisterComponentType<SGF_EntityNameComponent>();
	mComponentManager->RegisterComponentType<SGF_TransformComponent>();
	mComponentManager->RegisterComponentType<SGF_StaticMeshComponent>();
	mComponentManager->RegisterComponentType<SGF_SpotLightComponent>();
	mComponentManager->RegisterComponentType<SGF_PointLightComponent>();
	mComponentManager->RegisterComponentType<SGF_DirectionalLightComponent>();
	mComponentManager->RegisterComponentType<SGF_CameraComponent>();
	mComponentManager->RegisterComponentType<SGF_AtmosphereComponent>();
}
