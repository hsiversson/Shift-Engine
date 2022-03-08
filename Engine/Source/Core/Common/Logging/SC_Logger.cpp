#include "SC_Logger.h"

#if ENABLE_LOGGING

#include <sstream>
#include <iomanip>

SC_Logger* SC_Logger::gInstance = nullptr;

void SC_Logger::Create()
{
	assert(gInstance == nullptr);
	gInstance = new SC_Logger;
}

void SC_Logger::Destroy()
{
	delete gInstance;
	gInstance = nullptr;
}

void SC_Logger::LogMessage(const SC_LogType& aType, const std::string& aMsg, const char* aFunc /*= nullptr*/, bool aFlush /*= false*/)
{
	SC_LogMessage msg;
	msg.mMessage = aMsg;
	msg.mFunction = aFunc;
	msg.mType = aType;
	msg.mTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	gInstance->mThread.QueueMessage(msg);

	if (aFlush)
	{
		while (!gInstance->mThread.mQueue.empty())
			SC_Thread::Yield();
	}
}

SC_Logger::SC_Logger()
{
	mThread.SetName("SC_LoggerThread");
	mThread.Start();
}

SC_Logger::~SC_Logger()
{

}

SC_Logger::SC_LoggerThread::SC_LoggerThread()
{
	SetName("Log Thread");
	Start();
}

SC_Logger::SC_LoggerThread::~SC_LoggerThread()
{
	Stop(false);
	mHasWorkEvent.Signal();
	Wait();
}

void SC_Logger::SC_LoggerThread::QueueMessage(const SC_LogMessage& aMessage)
{
	SC_MutexLock lock(mQueueMutex);
	mQueue.push(aMessage);
	mHasWorkEvent.Signal();
}

void SC_Logger::SC_LoggerThread::ThreadMain()
{
	while (mIsRunning)
	{
		mHasWorkEvent.Wait();
		mHasWorkEvent.Reset();

		while (!mQueue.empty())
		{
			SC_LogMessage msg;
			{
				SC_MutexLock lock(mQueueMutex);
				msg = mQueue.front();
				mQueue.pop();
			}

			std::stringstream timeStr;

			struct tm buf;
			localtime_s(&buf, &msg.mTime);
			timeStr << std::put_time(&buf, "%Y-%m-%d %X");
			std::wstringstream sstr;
			sstr << L"[" << SC_UTF8ToUTF16(timeStr.str()) << L"] - " << SC_UTF8ToUTF16(msg.mMessage.c_str()) << std::endl;

			// If visual studio
			OutputDebugString(sstr.str().c_str());

			// If log file open
			// stream << sstr;
		}
	}
}

#endif //ENABLE_LOGGING