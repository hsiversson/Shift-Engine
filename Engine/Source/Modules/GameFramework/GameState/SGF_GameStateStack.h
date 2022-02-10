#pragma once

class SGF_GameState;
class SGF_GameStateStack
{
public:
	SGF_GameStateStack();
	~SGF_GameStateStack();

	bool Init();

	void Update();

	void Push(SGF_GameState* aState);
	void Pop();

private:
	SC_Array<SGF_GameState*> mStack;
};

