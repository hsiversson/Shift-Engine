#pragma once
#include "Application/SAF_Framework.h"
#include "Graphics/View/SGfx_Camera.h"

class SGF_World;
class SGfx_View;
struct SR_TaskEvent;

class GameInstance : public SAF_AppCallbacks
{
public:
	GameInstance();
	~GameInstance();

	virtual bool Init() override;
	virtual bool Update() override;
	virtual bool Render() override;
	virtual void Exit() override;

private:
	SC_Ref<SGF_World> mActiveWorld; 
	SC_Ref<SGfx_View> mView; 
	SC_Ref<SR_TaskEvent> copyEvent;
	SGfx_Camera mCamera;
};

