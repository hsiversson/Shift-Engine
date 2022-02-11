#pragma once

class SED_Panel
{
public:
	SED_Panel() = default;
	virtual ~SED_Panel() {}

	virtual void Update() {}
	virtual void OnRender() = 0;
};