#include "SGF_Component.h"

class SGF_EntityIdComponent : public SGF_Component
{
	COMPONENT_DECLARE(SGF_EntityIdComponent, "EntityIdComponent");
public:
	SGF_EntityIdComponent();
	~SGF_EntityIdComponent();

	const SC_UUID& GetUUID() const;

	//bool Save(SC_Json& aOutSaveData) const override;
	//bool Load(const SC_Json& aSavedData) override;
private:
	SC_UUID mUUID;
};
