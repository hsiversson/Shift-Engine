#pragma once

class SGfx_SceneGraph;
class SGfx_Model;
class SGfx_Light;
class SGfx_ParticleEffect;
class SGfx_Decal;
class SGfx_View;
class SGfx_Environment;
class SGfx_MeshInstance;
class SGfx_Renderer;


/*
*
*	Visual representation of game world
*
*/

class SGfx_World
{
public:
	SGfx_World();
	~SGfx_World();

	SC_Ref<SGfx_View> CreateView();
	void DestroyView(const SC_Ref<SGfx_View>& aView);

	/* Prepare stage, this is the point where we synchronize the game-state and copy over any interesting render data to the view so that we can render it at a later stage. */
	void PrepareView(SGfx_View* aView);

	/* Render stage, this is where we launch all render tasks that will process the render data and populate the GPU with work */
	void RenderView(SGfx_View* aView);

	void AddModel(SC_Ref<SGfx_Model> aModel);
	void RemoveModel(SC_Ref<SGfx_Model> aModel);

	void AddMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance);
	void RemoveMesh(SC_Ref<SGfx_MeshInstance> aMeshInstance);

	// AddCamera
	// RemoveCamera

	void AddLight(SC_Ref<SGfx_Light> aLight);
	void RemoveLight(SC_Ref<SGfx_Light> aLight);

	void AddParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect);
	void RemoveParticleEffect(SC_Ref<SGfx_ParticleEffect> aParticleEffect);

	void AddDecal(SC_Ref<SGfx_Decal> aDecal);
	void RemoveDecal(SC_Ref<SGfx_Decal> aDecal);

	SGfx_SceneGraph* GetSceneGraph() const;
	SGfx_Renderer* GetRenderer() const;
	SGfx_Environment* GetEnvironment() const;

private:
	SC_Array<SC_Ref<SGfx_View>> mViews;
	SC_UniquePtr<SGfx_SceneGraph> mSceneGraph;
	SC_UniquePtr<SGfx_Environment> mEnvironment;

	SC_UniquePtr<SGfx_Renderer> mRenderer;
};

