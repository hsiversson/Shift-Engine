#pragma once
#include "SED_Panel.h"
#include "GameFramework/Entity/Components/SGF_Component.h"

class SGF_World;
class SGF_Entity;

class SED_PropertiesPanel : public SED_Panel
{
public:
	SED_PropertiesPanel();
	SED_PropertiesPanel(const SC_Ref<SGF_World>& aWorld);
	~SED_PropertiesPanel();

	void OnRender() override;

	void SetSelectedEntity(SGF_Entity* aEntity);
	SGF_Entity* GetSelectedEntity() const;

private:
	void DrawComponent(const SGF_ComponentId& aComponentId, SGF_Entity* aEntity) const;

	void DrawProperty(SGF_PropertyHelperBase& aProperty) const;
	void DrawPropertyInternal(SGF_PropertyHelper<bool>& aProperty) const;
	void DrawPropertyInternal(SGF_PropertyHelper<int32>& aProperty) const;
	void DrawPropertyInternal(SGF_PropertyHelper<uint32>& aProperty) const;
	void DrawPropertyInternal(SGF_PropertyHelper<float>& aProperty) const;
	void DrawPropertyInternal(SGF_PropertyHelper<SC_Vector>& aProperty) const;
	void DrawPropertyInternal(SGF_PropertyHelper<SC_Color>& aProperty) const;
	void DrawPropertyInternal(SGF_PropertyHelper<SC_Quaternion>& aProperty) const;
	void DrawPropertyInternal(SGF_PropertyHelper<SC_Ref<SR_Texture>>& aProperty) const;
	void DrawPropertyInternal(SGF_PropertyHelper<SC_Ref<SGfx_MaterialInstance>>& aProperty) const;
	void DrawPropertyInternal(SGF_PropertyHelper<SC_Ref<SGfx_MeshInstance>>& aProperty) const;
	void DrawPropertyInternal(SGF_PropertyHelper<SC_Ref<SGF_Entity>>& aProperty) const;

	SGF_Entity* mSelectedEntity;

	SC_Ref<SGF_World> mWorld;
};

