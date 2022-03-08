#include "SGF_World.h"
#include "Graphics/World/SGfx_World.h"
//#include "../Editor/SED_AssimpImporter.h"

SGF_World::SGF_World()
{

}

SGF_World::~SGF_World()
{

}

bool SGF_World::Init()
{
	mGraphicsWorld = SC_MakeUnique<SGfx_World>();

	return true;
}

bool SGF_World::LoadLevel(const char* /*aLevel*/)
{
	SC_Ref<SGF_Level> level = SC_MakeRef<SGF_Level>();
	level->SetWorld(this);
	if (level->Load(SC_EnginePaths::Get().GetGameDataDirectory() + "/Levels/Sponza.slvl"))
		mLevels.Add(level);

	//SED_AssimpScene scene;
	//SED_AssimpImporter importer;
	////importer.ImportScene(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shapes/ShaderBall.fbx", scene);
	//importer.ImportScene(SC_EnginePaths::Get().GetGameDataDirectory() + "/Models/SponzaPBR/sponzaPBR.obj", scene, 0.01f);
	//
	//scene.ConvertToLevelAndSave(*level);
	////level->Save(SC_EnginePaths::Get().GetGameDataDirectory() + "/Levels/Sponza.slvl");
	//mLevels.Add(level);

	return true;
}

void SGF_World::Update()
{
	for (SC_Ref<SGF_Level>& level : mLevels)
		level->Update();
}

void SGF_World::AddLevel(const SC_Ref<SGF_Level>& aLevel)
{
	mLevels.Add(aLevel);
}

void SGF_World::RemoveLevel()
{

}

SGfx_World* SGF_World::GetGraphicsWorld() const
{
	return mGraphicsWorld.get();
}
