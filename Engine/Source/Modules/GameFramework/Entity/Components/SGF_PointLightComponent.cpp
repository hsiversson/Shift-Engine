#include "SGF_PointLightComponent.h"
#include "SGF_TransformComponent.h"
#include "GameFramework/Entity/SGF_Entity.h"

#include "../../GameWorld/SGF_World.h"
#include "Graphics/World/SGfx_World.h"

SGF_PointLightComponent::SGF_PointLightComponent()
	: mColor({ 255, 255, 255, 255 })
	, mRange(10.0f)
	, mIntensity(10.0f)
	, mSourceRadius(0.0f)
	, mSourceRadiusSoft(0.0f)
{
	mPointLight = SC_MakeRef<SGfx_PointLight>();
}

SGF_PointLightComponent::~SGF_PointLightComponent()
{

}

void SGF_PointLightComponent::OnCreate()
{
	if (SGF_Entity* entity = GetParentEntity())
		entity->GetWorld()->GetGraphicsWorld()->AddLight(mPointLight);
}

void SGF_PointLightComponent::OnUpdate()
{
	SGF_TransformComponent* transformComp = GetParentEntity()->GetComponent<SGF_TransformComponent>();
	if (transformComp)
	{
		SC_Matrix mTransform = transformComp->GetTransform();
		mPointLight->SetPosition(mTransform.GetPosition());
		mPointLight->SetBoundingSphere({ mTransform.GetPosition(), mRange });
	}

	SC_LinearColor col = SC_ConvertColorToLinear(mColor);
	mPointLight->SetColor({ col.r, col.g, col.b, col.a });
	mPointLight->SetSourceRadius(mSourceRadius);
	mPointLight->SetSourceRadiusSoft(mSourceRadiusSoft);
	mPointLight->SetRange(mRange);
	mPointLight->SetIntensity(mIntensity);
}
