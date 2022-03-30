#pragma once
#include "GameFramework/Entity/Components/SGF_Component.h"

class SGF_EntityGraph;
class SGF_World;

class SGF_Entity
{
	friend class SED_WorldHierarchyPanel;
public:
	SGF_Entity();
	~SGF_Entity();

	void Update();

	void SetWorld(SGF_World* aWorld);
	SGF_World* GetWorld() const;

	template<class ComponentType>
	bool HasComponent() const;
	bool HasComponent(const SGF_ComponentId& aComponentId);

	template<class ComponentType>
	ComponentType* GetComponent() const;
	SGF_Component* GetComponent(const SGF_ComponentId& aComponentId);

	template<class ComponentType>
	ComponentType* AddComponent();

	void AddComponent(SC_Ref<SGF_Component> aComponent);

	bool Is(const SC_UUID& aId) const;

	const SC_Array<SGF_Entity*>& GetChildren() const;
	void SetParent(SGF_Entity* aParent);
	SGF_Entity* GetParent() const;

	void SetName(const std::string& aName);
	const std::string& GetName() const;

	bool Save(SC_Json& aOutSaveData) const;
	bool Load(const SC_Json& aSavedData);

private:
	void AddChild(SGF_Entity* aEntity);
	void RemoveChild(SGF_Entity* aEntity);

	SC_Array<SGF_Entity*> mChildren;
	SGF_Entity* mParent;
	SGF_World* mWorld;

	SC_UnorderedMap<SGF_ComponentId, SC_Ref<SGF_Component>> mComponents;
	//SC_Array<SC_Ref<SGF_Component>> mComponents;

	//SC_UniquePtr<SGF_EntityGraph> mEntityGraph;

	std::string mName;
};

template<class ComponentType>
inline bool SGF_Entity::HasComponent() const
{
	return mComponents.find(ComponentType::Id()) != mComponents.end();
}

template<class ComponentType>
inline ComponentType* SGF_Entity::GetComponent() const
{
	if (HasComponent<ComponentType>())
		return static_cast<ComponentType*>(mComponents.at(ComponentType::Id()).get());

	return nullptr;
}

template<class ComponentType>
inline ComponentType* SGF_Entity::AddComponent()
{
	if (HasComponent<ComponentType>())
		return static_cast<ComponentType*>(mComponents.at(ComponentType::Id()).get());

	mComponents[ComponentType::Id()] = SC_MakeRef<ComponentType>();
	mComponents[ComponentType::Id()]->SetParentEntity(this);
	mComponents[ComponentType::Id()]->OnCreate();
	return static_cast<ComponentType*>(mComponents[ComponentType::Id()].get());
}
