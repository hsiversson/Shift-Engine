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
	void DrawProperty(const SGF_PropertyBase::Type& aType, void* aData, const char* aPropertyName, const void* aResetData) const;
	void DrawPropertyInternal(const char* aName, bool& aProperty) const;
	void DrawPropertyInternal(const char* aName, int32& aProperty) const;
	void DrawPropertyInternal(const char* aName, uint32& aProperty) const;
	void DrawPropertyInternal(const char* aName, float& aProperty) const;
	void DrawPropertyInternal(const char* aName, SC_Vector& aProperty, const SC_Vector& aResetValue) const;
	void DrawPropertyInternal(const char* aName, SC_Color& aProperty) const;
	void DrawPropertyInternal(const char* aName, SC_Quaternion& aProperty, const SC_Quaternion& aResetValue) const;
	void DrawPropertyInternal(const char* aName, SC_Ref<SR_Texture>& aProperty) const;
	void DrawPropertyInternal(const char* aName, SC_Ref<SGfx_MaterialInstance>& aProperty) const;
	void DrawPropertyInternal(const char* aName, SC_Ref<SGfx_MeshInstance>& aProperty) const;
	void DrawPropertyInternal(const char* aName, SC_Ref<SGF_Entity>& aProperty) const;

	SGF_Entity* mSelectedEntity;

	SC_Ref<SGF_World> mWorld;
};

