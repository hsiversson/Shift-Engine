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
