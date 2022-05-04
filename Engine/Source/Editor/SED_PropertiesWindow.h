#pragma once
#include "SED_Window.h"
#include "GameFramework/Entity/SGF_Entity.h"
#include "GameFramework/Entity/Components/SGF_Component.h"

class SGF_World;
class SGF_Entity;

class SED_PropertiesWindow : public SED_Window
{
public:
	SED_PropertiesWindow();
	SED_PropertiesWindow(const SC_Ref<SGF_World>& aWorld);
	~SED_PropertiesWindow();

	void SetSelectedEntity(const SGF_Entity& aEntity);
	const SGF_Entity& GetSelectedEntity() const;

	const char* GetWindowName() const override { return "Properties"; }
protected:
	void OnDraw() override;

private:
	void DrawComponent(const SGF_ComponentId& aComponentId, const SGF_Entity& aEntity);

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
	void DrawPropertyInternal(SGF_PropertyHelper<SGF_EntityHandle>& aProperty) const;

	SGF_Entity mSelectedEntity;

	SC_Ref<SGF_World> mWorld;
	float mPropertyNameColumnWidth;
};

