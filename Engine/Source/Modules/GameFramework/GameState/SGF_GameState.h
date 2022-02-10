#pragma once

class SGF_GameState
{
public:
	SGF_GameState();
	virtual ~SGF_GameState();

	virtual void Activate() {}
	virtual void Deactivate() {}

	virtual void Update() {}

protected:

};

