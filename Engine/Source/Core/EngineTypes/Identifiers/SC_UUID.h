#pragma once
#include <string>

class SC_UUID
{
public:
	SC_UUID();
	~SC_UUID();

	bool operator==(const SC_UUID& aOther) const;
	bool operator!=(const SC_UUID& aOther) const;

	void AsString(std::string& aOutString) const;

	struct Components
	{
		uint64 mLowComp;
		uint64 mHighComp;
	};

	Components mUUID;
};

