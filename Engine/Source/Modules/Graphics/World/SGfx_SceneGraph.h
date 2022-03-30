#pragma once

class SGfx_Model;
class SGfx_Light;
class SGfx_ParticleEffect;
class SGfx_Decal;
class SGfx_View;
class SGfx_MeshInstance;
class SGfx_World;

/*
*
*	Handles per-view culling of visual representation of game world
*	Internally structures objects in various spatial structures and acceleration structures
* 
*/

class SGfx_SceneGraph
{
public:
	SGfx_SceneGraph(SGfx_World* aParentWorld);
	~SGfx_SceneGraph();

	void PrepareView(SGfx_View* aView);

	void AddModel(SC_Ref<SGfx_Model> aModel);
	void RemoveModel(SC_Ref<SGfx_Model> aModel);

	void AddMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance);
	void RemoveMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance);

	void AddLight(SC_Ref<SGfx_Light> aLight);
	void RemoveLight(SC_Ref<SGfx_Light> aLight);

	void AddParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect);
	void RemoveParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect);

	void AddDecal(SC_Ref<SGfx_Decal> aDecal);
	void RemoveDecal(SC_Ref<SGfx_Decal> aDecal);

private:
	void CullMeshes(SGfx_View* aView);
	void CullLights(SGfx_View* aView);

	void AddRaytracingGeometry(SGfx_View* aView);

	SC_Mutex mMeshesMutex;
	SC_Array<SC_Ref<SGfx_MeshInstance>> mMeshes;

	SC_Mutex mLightsMutex;
	SC_Array<SC_Ref<SGfx_Light>> mLights;

	SC_Mutex mParticlesMutex;
	SC_Array<SC_Ref<SGfx_ParticleEffect>> mParticleEffects;

	SC_Mutex mDecalsMutex;
	SC_Array<SC_Ref<SGfx_Decal>> mDecals;

	SGfx_World* mParentWorld;
};

