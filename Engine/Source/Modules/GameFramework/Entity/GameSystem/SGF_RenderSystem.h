#pragma once
#include "GameFramework/Entity/SGF_GameSystem.h"

class SGfx_World;
class SGF_RenderSystem : public SGF_GameSystem
{
public:
	SGF_RenderSystem();
	~SGF_RenderSystem();

	bool Init() override;
	void Update() override;

	SGfx_World* GetGraphicsWorld() const;

private:

	void UpdateStaticMeshes();
	void UpdateLights();

	SC_UniquePtr<SGfx_World> mGraphicsWorld;
};

