#include "SGF_TransformComponent.h"

SGF_TransformComponent::SGF_TransformComponent() 
	: mPosition(0.0f)
	, mRotation(SC_Quaternion::Identity())
	, mScale(1.0f)
{
}

SGF_TransformComponent::~SGF_TransformComponent()
{
}

SC_Matrix SGF_TransformComponent::GetTransform() const
{
	SC_ScaleMatrix scale(mScale.Get());
	SC_Matrix rotation = mRotation.Get().AsMatrix();
	SC_TranslationMatrix translation(mPosition.Get());
	return (scale * rotation) * translation;
}

bool SGF_TransformComponent::Save(SC_Json& aOutSaveData) const
{
	if (!SGF_Component::Save(aOutSaveData))
		return false;
	
	SC_Json positionData;
	positionData["x"] = mPosition.Get().x;
	positionData["y"] = mPosition.Get().y;
	positionData["z"] = mPosition.Get().z;

	SC_Json rotationData;
	rotationData["x"] = mRotation.Get().x;
	rotationData["y"] = mRotation.Get().y;
	rotationData["z"] = mRotation.Get().z;
	rotationData["w"] = mRotation.Get().w;

	SC_Json scaleData;
	scaleData["x"] = mScale.Get().x;
	scaleData["y"] = mScale.Get().y;
	scaleData["z"] = mScale.Get().z;

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
