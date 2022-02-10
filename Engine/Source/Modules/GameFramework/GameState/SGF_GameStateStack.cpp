#include "SGF_GameStateStack.h"
#include "SGF_GameState.h"

SGF_GameStateStack::SGF_GameStateStack()
{

}

SGF_GameStateStack::~SGF_GameStateStack()
{

}

bool SGF_GameStateStack::Init()
{
	return false;
}

void SGF_GameStateStack::Update()
{
	SGF_GameState* state = mStack.Last();
	state->Update();
}

void SGF_GameStateStack::Push(SGF_GameState* aState)
{
	mStack.Add(aState);
	aState->Activate();
}

void SGF_GameStateStack::Pop()
{
	SGF_GameState* state = mStack.Last();
	state->Deactivate();
	mStack.RemoveLast();
}
