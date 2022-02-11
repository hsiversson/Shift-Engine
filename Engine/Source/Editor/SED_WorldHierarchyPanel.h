#pragma once
#include "SED_Panel.h"

class SGF_World;
class SGF_Entity;

class SED_WorldHierarchyPanel : public SED_Panel
{
public:
	SED_WorldHierarchyPanel() = default;
	SED_WorldHierarchyPanel(const SC_Ref<SGF_World>& aWorld);
	~SED_WorldHierarchyPanel();

	void OnRender() override;

	SGF_Entity* GetSelected() const;

private:

	void DrawEntityNode(SGF_Entity* aEntity);

	SC_Ref<SGF_World> mWorld;

	SGF_Entity* mSelectedEntity;
};

