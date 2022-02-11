#pragma once
#include "SED_Panel.h"

class SGF_World;
class SGF_Entity;

class SED_PropertiesPanel : public SED_Panel
{
public:
	SED_PropertiesPanel();
	~SED_PropertiesPanel();

	void OnRender() override;

	void SetSelectedEntity(SGF_Entity* aEntity);
	SGF_Entity* GetSelectedEntity() const;

private:
	SGF_Entity* mSelectedEntity;
};

