#include "SGF_ECSModule.h"

#include "GameFramework/Entity/Components/SGF_TransformComponent.h"
#include "GameFramework/Entity/Components/SGF_StaticMeshComponent.h"
#include "GameFramework/Entity/Components/SGF_SpotLightComponent.h"

SGF_ECSModule::SGF_ECSModule()
{

}

SGF_ECSModule::~SGF_ECSModule()
{

}

void SGF_ECSModule::RegisterComponents()
{
	SGF_Component::RegisterComponent<SGF_TransformComponent>();
	SGF_Component::RegisterComponent<SGF_StaticMeshComponent>();
	SGF_Component::RegisterComponent<SGF_SpotLightComponent>();
}
