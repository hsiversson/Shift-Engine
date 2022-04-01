#include "SGF_SpotLightComponent.h"
#include "SGF_TransformComponent.h"
#include "GameFramework/Entity/SGF_Entity.h"
#include "GameFramework/GameWorld/SGF_World.h"
#include "Graphics/World/SGfx_World.h"

SGF_SpotLightComponent::SGF_SpotLightComponent()
	: mColor({ 255, 255, 255, 255 })
	, mRange(10.0f)
	, mIntensity(10.0f)
	, mSourceRadius(0.0f)
	, mSourceRadiusSoft(0.0f)
	, mInnerAngle(0.0f)
	, mOuterAngle(45.0f)
{
	mSpotLight = SC_MakeRef<SGfx_SpotLight>();
}

SGF_SpotLightComponent::~SGF_SpotLightComponent()
{

}

void SGF_SpotLightComponent::OnCreate()
{
	if (SGF_Entity* entity = GetParentEntity())
		entity->GetWorld()->GetGraphicsWorld()->AddLight(mSpotLight);
}

void SGF_SpotLightComponent::OnUpdate()
{
	SGF_TransformComponent* transformComp = GetParentEntity()->GetComponent<SGF_TransformComponent>();
	if (transformComp)
	{
		SC_Matrix mTransform = transformComp->GetTransform();
		mSpotLight->SetPosition(mTransform.GetPosition());
		mSpotLight->SetDirection(mTransform.GetForward());
		mSpotLight->SetBoundingSphere({ mTransform.GetPosition(), mRange });
	}

	SC_LinearColor col = SC_ConvertColorToLinear(mColor);
	mSpotLight->SetColor({ col.r, col.g, col.b, col.a });
	mSpotLight->SetInnerAngle(mInnerAngle);
	mSpotLight->SetOuterAngle(mOuterAngle);
	mSpotLight->SetSourceRadius(mSourceRadius);
	mSpotLight->SetSourceRadiusSoft(mSourceRadiusSoft);
	mSpotLight->SetRange(mRange);
	mSpotLight->SetIntensity(mIntensity);
}

void SGF_SpotLightComponent::OnDestroy()
{
	if (SGF_Entity* entity = GetParentEntity())
		entity->GetWorld()->GetGraphicsWorld()->RemoveLight(mSpotLight);
}
