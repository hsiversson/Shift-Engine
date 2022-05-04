#include "Common/Module/SC_Module.h"

class SGF_ECSModule : public SC_Module
{
	SC_MODULE_DECLARE(SGF_ECSModule, "GameFramework.Entities", "");
	SC_MODULE_DEPENDENCIES(Graphics.World, Graphics.Lights);
public:
	SGF_ECSModule();
	~SGF_ECSModule();

	bool OnLoad() override;
	bool OnUnload() override;

	static void RegisterComponents();
};
