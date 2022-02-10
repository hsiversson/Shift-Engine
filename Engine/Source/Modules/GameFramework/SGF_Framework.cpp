#include "SGF_Framework.h"

SGF_Framework::SGF_Framework()
	: mGameInstance(nullptr)
{

}

SGF_Framework::~SGF_Framework()
{

}

int32 SGF_Framework::InternalMain(SGF_GameInterface* aGameInstance)
{
	SC_Thread::GetMainThreadId();
#if ENABLE_LOGGING
	SC_Logger::Create();
#endif
	SC_ThreadPool::Create();
#if ENABLE_CONSOLE
	SC_Console::Create();
#endif

	mGameInstance = aGameInstance;
	if (mGameInstance->Init())
	{
		MainLoop();
	}

	Exit();

#if ENABLE_CONSOLE
	SC_Console::Destroy();
#endif
	SC_ThreadPool::Destroy();
#if ENABLE_LOGGING
	SC_Logger::Destroy();
#endif

	return 0;
}

bool SGF_Framework::Init()
{
	return false;
}

void SGF_Framework::MainLoop()
{
	while (Tick());
}

bool SGF_Framework::Tick()
{

	mGameInstance->Update();

	return true;
}

void SGF_Framework::Exit()
{

}
