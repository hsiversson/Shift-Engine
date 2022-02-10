#pragma once

class SC_ConfigManager
{
public:
	SC_ConfigManager();
	~SC_ConfigManager();

	bool Init();
private:
	void EnsureConfigExistance();
};

