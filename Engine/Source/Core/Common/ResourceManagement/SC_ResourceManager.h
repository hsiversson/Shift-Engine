#pragma once	
#include "InputOutput/SRP/SC_SRP.h"

class SC_ResourceManager
{
public:
	SC_ResourceManager();
	virtual ~SC_ResourceManager();

	bool LoadSRP(const SC_FilePath& aSRPPath, SC_SRPData& aOutData);
	bool SaveSRP(const SC_FilePath& aOutSRPPath, const SC_SRPData& aData);

private:
};