#include "SC_Base.h"

#include "SGF_Framework.h"
#include "GameInstance.h"

int APIENTRY WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hInstPrev*/, PSTR /*cmdline*/, int /*cmdshow*/)
{

	// Initialize engine
	//		Create window
	//		Create subsystems (render, physics, audio etc)
	// Link game application to engine
	//		Engine doesn't know about game but still needs to call functionality in it
	// 
	// Run game loop
	// 
	// Unload all resources
	// Uninitialize engine
	// Exit

	SGF_Framework frameworkInstance;
	GameInstance gameInstance;

	int32 returnCode = frameworkInstance.InternalMain(&gameInstance);

	return returnCode;
}

