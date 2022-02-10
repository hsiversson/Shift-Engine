#pragma once

#if ENABLE_LOGGING

#include "SC_Thread.h"
#include "SC_Event.h"
#include "SC_Mutex.h"
#include <queue>

enum class SC_LogType
{
	Default,
	Info,
	Warning,
	Error,
	Critical
};

class SC_Logger
{
public:
	static void Create();
	static void Destroy();
	static SC_Logger* Get() { return gInstance; }

	static void LogMessage(const SC_LogType& aType, const std::string& aMsg, const char* aFunc = nullptr);

private:
	SC_Logger();
	~SC_Logger();

private:
	struct SC_LogMessage
	{
		std::string mMessage;
		std::string mFunction;
		std::time_t mTime;
		SC_LogType mType;
	};

	class SC_LoggerThread : public SC_Thread
	{
	public:
		SC_LoggerThread();
		~SC_LoggerThread();

		void QueueMessage(const SC_LogMessage& aMessage);

	protected:
		void ThreadMain() override;

	private:
		SC_Event mHasWorkEvent;
		SC_Mutex mQueueMutex;
		std::queue<SC_LogMessage> mQueue;
	};
private:
	SC_LoggerThread mThread;

	static SC_Logger* gInstance;
};

#define SC_LOG_CATEGORY(aType, aMsg, ...)	SC_Logger::Get()->LogMessage(aType,					SC_FormatStr(aMsg, __VA_ARGS__), __FUNCTION__)
#define SC_LOG(aMsg, ...)					SC_Logger::Get()->LogMessage(SC_LogType::Info,		SC_FormatStr(aMsg, __VA_ARGS__), __FUNCTION__)
#define SC_ERROR(aMsg, ...)					SC_Logger::Get()->LogMessage(SC_LogType::Error,		SC_FormatStr(aMsg, __VA_ARGS__), __FUNCTION__)
#define SC_WARNING(aMsg, ...)				SC_Logger::Get()->LogMessage(SC_LogType::Warning,	SC_FormatStr(aMsg, __VA_ARGS__), __FUNCTION__)

#else //ENABLE_LOGGING

#define SC_LOG_CATEGORY(aType, aMsg, ...)
#define SC_LOG(aMsg, ...)
#define SC_ERROR(aMsg, ...)
#define SC_WARNING(aMsg, ...)

#endif //ENABLE_LOGGING