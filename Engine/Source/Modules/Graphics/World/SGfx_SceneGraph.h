#pragma once
#include "Graphics/Mesh/SGfx_MeshInstance.h"

class SGfx_Model;
class SGfx_Light;
class SGfx_ParticleEffect;
class SGfx_Decal;
class SGfx_View;
class SGfx_World;

enum class SGfx_SceneGraphActionType
{
	None,
	Add,
	Remove,
	Move,
	Activate,
	Inactivate
};

enum class SGfx_SceneGraphObjectType
{
	Unknown,
	Mesh,
	Light,
	ParticleEffect,
	Decal
};

struct SGfx_SceneGraphAction
{
	SGfx_SceneGraphAction()
		: mActionType(SGfx_SceneGraphActionType::None)
		, mObjectType(SGfx_SceneGraphObjectType::Unknown)
		, mMeshInstance(nullptr)
	{}

	SGfx_SceneGraphAction(const SGfx_SceneGraphAction& aOther)
	{
		SC_Memcpy(this, &aOther, sizeof(SGfx_SceneGraphAction));
	}

	~SGfx_SceneGraphAction() {}

	SGfx_SceneGraphActionType mActionType;
	SGfx_SceneGraphObjectType mObjectType;
	union
	{
		SC_Ref<SGfx_MeshInstance> mMeshInstance;
		SC_Ref<SGfx_Light> mLight;
		SC_Ref<SGfx_ParticleEffect> mParticleEffect;
		SC_Ref<SGfx_Decal> mDecal;
	};
};

/*
*	Handles per-view culling of visual representation of game world
*	Internally structures objects in various spatial structures and acceleration structures
*/
class SGfx_SceneGraph
{
public:
	SGfx_SceneGraph(SGfx_World* aParentWorld);
	~SGfx_SceneGraph();

	void Update();

	void PrepareView(SGfx_View* aView);

	void AddModel(SC_Ref<SGfx_Model> aModel);
	void RemoveModel(SC_Ref<SGfx_Model> aModel);

	void AddMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance);
	void RemoveMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance);
	void SetActive(SC_Ref<SGfx_MeshInstance> aMeshInstance, bool aValue);

	void AddLight(SC_Ref<SGfx_Light> aLight);
	void RemoveLight(SC_Ref<SGfx_Light> aLight);
	void SetActive(SC_Ref<SGfx_Light> aLight, bool aValue);

	void AddParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect);
	void RemoveParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect);
	void SetActive(SC_Ref<SGfx_ParticleEffect> aParticleEffect, bool aValue);

	void AddDecal(SC_Ref<SGfx_Decal> aDecal);
	void RemoveDecal(SC_Ref<SGfx_Decal> aDecal);
	void SetActive(SC_Ref<SGfx_Decal> aDecal, bool aValue);

private:
	void FlushPendingActions();

	void Add_Internal(SC_Ref<SGfx_MeshInstance> aMeshInstance);
	void Add_Internal(SC_Ref<SGfx_Light> aLight);
	void Add_Internal(SC_Ref<SGfx_ParticleEffect> aParticleEffect);
	void Add_Internal(SC_Ref<SGfx_Decal> aDecal);

	void Remove_Internal(SC_Ref<SGfx_MeshInstance> aMeshInstance);
	void Remove_Internal(SC_Ref<SGfx_Light> aLight);
	void Remove_Internal(SC_Ref<SGfx_ParticleEffect> aParticleEffect);
	void Remove_Internal(SC_Ref<SGfx_Decal> aDecal);

	void Move_Internal(SC_Ref<SGfx_MeshInstance> aMeshInstance);
	void Move_Internal(SC_Ref<SGfx_Light> aLight);
	void Move_Internal(SC_Ref<SGfx_ParticleEffect> aParticleEffect);
	void Move_Internal(SC_Ref<SGfx_Decal> aDecal);

	void SetActive_Internal(SC_Ref<SGfx_MeshInstance> aMeshInstance, bool aValue);
	void SetActive_Internal(SC_Ref<SGfx_Light> aLight, bool aValue);
	void SetActive_Internal(SC_Ref<SGfx_ParticleEffect> aParticleEffect, bool aValue);
	void SetActive_Internal(SC_Ref<SGfx_Decal> aDecal, bool aValue);

	void CullMeshes(SGfx_View* aView);
	void CullLights(SGfx_View* aView);

	void AddRaytracingGeometry(SGfx_View* aView);

	SC_Mutex mPendingActionsMutex;
	SC_Array<SGfx_SceneGraphAction> mPendingActions;

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

