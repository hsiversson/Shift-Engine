#include "SGF_ECSModule.h"

#include "Components/SGF_TransformComponent.h"
#include "Components/SGF_StaticMeshComponent.h"
#include "Components/SGF_DirectionalLightComponent.h"
#include "Components/SGF_PointLightComponent.h"
#include "Components/SGF_SpotLightComponent.h"
#include "Components/SGF_AtmosphereComponent.h"
#include "Components/SGF_CameraComponent.h"
#include "Components/SGF_EntityIdComponent.h"

SGF_ECSModule::SGF_ECSModule()
{

}

SGF_ECSModule::~SGF_ECSModule()
{

}

void SGF_ECSModule::RegisterComponents()
{
	SGF_Component::RegisterComponent<SGF_EntityIdComponent>();
	SGF_Component::RegisterComponent<SGF_TransformComponent>();
	SGF_Component::RegisterComponent<SGF_StaticMeshComponent>();
	SGF_Component::RegisterComponent<SGF_SpotLightComponent>();
	SGF_Component::RegisterComponent<SGF_PointLightComponent>();
	SGF_Component::RegisterComponent<SGF_DirectionalLightComponent>();
	SGF_Component::RegisterComponent<SGF_CameraComponent>();
	SGF_Component::RegisterComponent<SGF_AtmosphereComponent>();
}
