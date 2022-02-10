#include "Common/Module/SC_Module.h"

class SGF_ECSModule : public SC_Module
{
public:
	SGF_ECSModule();
	~SGF_ECSModule();

	static void RegisterComponents();
};
