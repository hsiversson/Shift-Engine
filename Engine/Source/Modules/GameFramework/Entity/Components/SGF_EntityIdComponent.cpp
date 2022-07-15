#include "SGF_EntityIdComponent.h"

SGF_EntityIdComponent::SGF_EntityIdComponent()
{

}

SGF_EntityIdComponent::~SGF_EntityIdComponent()
{

}

const SC_UUID& SGF_EntityIdComponent::GetUUID() const
{
	return mUUID;
}

bool SGF_EntityIdComponent::Save(SC_Json& aOutSaveData) const
{
	if (!SGF_Component::Save(aOutSaveData))
		return false;

	SC_Json idData;
	idData[0] = mUUID.mUUID.mLowComp;
	idData[1] = mUUID.mUUID.mHighComp;

	aOutSaveData["UUID"] = idData;
	return true;
}

bool SGF_EntityIdComponent::Load(const SC_Json& aSavedData)
{
	if (!SGF_Component::Load(aSavedData))
		return false;

	const SC_Json& idData = aSavedData["UUID"];
	mUUID.mUUID.mLowComp = idData[0].get<uint64>();
	mUUID.mUUID.mHighComp = idData[1].get<uint64>();

	return true;
}

SGF_EntityNameComponent::SGF_EntityNameComponent()
	: mName("Unnamed")
{

}

SGF_EntityNameComponent::~SGF_EntityNameComponent()
{

}

bool SGF_EntityNameComponent::Save(SC_Json& aOutSaveData) const
{
	if (!SGF_Component::Save(aOutSaveData))
		return false;

	aOutSaveData["Name"] = mName;
	return true;
}

bool SGF_EntityNameComponent::Load(const SC_Json& aSavedData)
{
	if (!SGF_Component::Load(aSavedData))
		return false;

	mName = aSavedData["Name"].get<std::string>();
	return true;
}
