#include "SC_GlobalData.h"

SC_ALIGN(16) uint8 _gGlobalDataInstance[sizeof(SC_GlobalData)];

class SC_GlobalDataLifetimeManager
{
public:
	SC_GlobalDataLifetimeManager()
	{
		SC_Construct(&SC_GetGlobalData());
	}
	~SC_GlobalDataLifetimeManager()
	{
		SC_Destruct(&SC_GetGlobalData());
	}
};

SC_GlobalData::SC_GlobalData()
{

}

void SC_InitGlobalData()
{
	static bool initialized = false;
	if (!initialized)
	{
		static SC_GlobalDataLifetimeManager gInstance;
		initialized = true;
	}
}
