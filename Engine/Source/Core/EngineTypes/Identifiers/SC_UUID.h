#pragma once

class SC_UUID
{
public:
	SC_UUID();
	SC_UUID(const SC_UUID&) = default;
	~SC_UUID();

	bool operator==(const SC_UUID& aOther);
	bool operator!=(const SC_UUID& aOther);

private:
	struct Components
	{
		uint64 mLowComp;
		uint64 mHighComp;
	};

	Components mUUID;
};

