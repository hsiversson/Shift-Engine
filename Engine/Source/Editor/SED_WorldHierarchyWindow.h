#pragma once
#include "SED_Window.h"
#include "GameFramework/Entity/SGF_Entity.h"

class SGF_World;

class SED_WorldHierarchyWindow : public SED_Window
{
public:
	SED_WorldHierarchyWindow() = default;
	SED_WorldHierarchyWindow(const SC_Ref<SGF_World>& aWorld);
	~SED_WorldHierarchyWindow();

	const SGF_Entity& GetSelected() const;

	const char* GetWindowName() const override { return "World Hierarchy"; }

protected:
	void OnDraw() override;

private:
	void DrawEntityNode(const SGF_Entity& aEntity);

	SC_Ref<SGF_World> mWorld;

	SGF_Entity mSelectedEntity;
};

