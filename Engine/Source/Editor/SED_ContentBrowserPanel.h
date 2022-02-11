#pragma once
#include "SED_Panel.h"

class SED_ContentBrowserPanel : public SED_Panel
{
public:
	SED_ContentBrowserPanel();
	~SED_ContentBrowserPanel();

	void OnRender() override;
};

