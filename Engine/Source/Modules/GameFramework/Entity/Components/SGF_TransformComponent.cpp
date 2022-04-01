#include "SGF_TransformComponent.h"
#include "GameFramework/Entity/SGF_Entity.h"

SGF_TransformComponent::SGF_TransformComponent() 
	: mPosition(0.0f)
	, mRotation(SC_Quaternion::Identity())
	, mScale(1.0f)
	, mIsStatic(true)
{
}

SGF_TransformComponent::~SGF_TransformComponent()
{
}

SC_Matrix SGF_TransformComponent::GetTransform() const
{
	SC_ScaleMatrix scale(mScale);
	SC_Matrix rotation = mRotation.AsMatrix();
	SC_TranslationMatrix translation(mPosition);

	SC_Matrix fullMatrix;
	if (SGF_Entity* parent = GetParentEntity()->GetParent())
	{
		fullMatrix = parent->GetComponent<SGF_TransformComponent>()->GetTransform() * ((scale * rotation) * translation);
	}
	else
		fullMatrix = (scale * rotation) * translation;

	return fullMatrix;
}

bool SGF_TransformComponent::Save(SC_Json& aOutSaveData) const
{
	if (!SGF_Component::Save(aOutSaveData))
		return false;
	
	SC_Json positionData;
	positionData["x"] = mPosition.x;
	positionData["y"] = mPosition.y;
	positionData["z"] = mPosition.z;

	SC_Json rotationData;
	rotationData["x"] = mRotation.x;
	rotationData["y"] = mRotation.y;
	rotationData["z"] = mRotation.z;
	rotationData["w"] = mRotation.w;

	SC_Json scaleData;
	scaleData["x"] = mScale.x;
	scaleData["y"] = mScale.y;
	scaleData["z"] = mScale.z;

	aOutSaveData["Position"] = positionData;
	aOutSaveData["Rotation"] = rotationData;
	aOutSaveData["Scale"] = scaleData;

	return true;
}

bool SGF_TransformComponent::Load(const SC_Json& aSavedData)
{
	if (!SGF_Component::Load(aSavedData))
		return false;

	const SC_Json& positionData = aSavedData["Position"];
	mPosition = SC_Vector(positionData["x"].get<float>(), positionData["y"].get<float>(), positionData["z"].get<float>());

	const SC_Json& rotationData = aSavedData["Rotation"];
	mRotation = SC_Quaternion(rotationData["x"].get<float>(), rotationData["y"].get<float>(), rotationData["z"].get<float>(), rotationData["w"].get<float>());

	const SC_Json& scaleData = aSavedData["Scale"];
	mScale = SC_Vector(scaleData["x"].get<float>(), scaleData["y"].get<float>(), scaleData["z"].get<float>());

	return true;
}
