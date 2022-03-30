#include "SGF_SpotLightComponent.h"
#include "SGF_TransformComponent.h"
#include "GameFramework/Entity/SGF_Entity.h"

SGF_SpotLightComponent::SGF_SpotLightComponent()
{
	mSpotLight = SC_MakeRef<SGfx_SpotLight>();
}

SGF_SpotLightComponent::~SGF_SpotLightComponent()
{

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