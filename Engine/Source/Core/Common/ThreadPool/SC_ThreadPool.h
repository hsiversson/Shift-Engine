#pragma once
#include "Platform/Async/SC_Future.h"
#include "Platform/Async/SC_Event.h"

// TEMP
#include <atomic>
#include <queue>
#include <functional>
#include <type_traits>

class SC_ThreadPool
{
public:
	static bool Create();
	static void Destroy();
	static SC_ThreadPool& Get();

public:
	uint32 GetTotalTaskCount() const;
	uint32 GetQueuedTaskCount() const;
	uint32 GetRunningTaskCount() const;
	uint32 GetThreadCount() const;

	bool IsPaused() const;

	void Pause();
	void Resume();

	template<class Func>
	void QueueTask(const Func& aTask)
	{
		// Should perhaps assert. Do we want to allow task threads to schedule more task work? 
		// Maybe yes, but only if the work is not waited for in the task thread.
		if (SC_Thread::gIsTaskThread) 
		{
			aTask();
		}
		else
		{
			++mTotalTaskCount;
			SC_MutexLock lock(mQueueMutex);
			mTasks.push(SC_Function<void()>(aTask));
			mHasWorkEvent.Signal();
		}
	}

	template <class Func, class... Args>
	void QueueTask(const Func& aTask, const Args&...aArgs)
	{
		auto task = [aTask, aArgs...]()
		{
			aTask(aArgs...);
		};
		QueueTask(task);
	}

	template <class Func, class... Args, class = std::enable_if_t<std::is_void_v<std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>>>>
	SC_Future<bool> SubmitTask(const Func& aTask, const Args&...aArgs)
	{
		SC_Ref<SC_Promise<bool>> promise = SC_MakeRef<SC_Promise<bool>>();
		SC_Future<bool> future = promise->GetFuture();
		QueueTask([aTask, aArgs..., promise]
			{
				 aTask(aArgs...);
				 promise->SetValue(true);
			});
		return future;
	}

	template <class Func, class... Args, class ReturnValue = std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>, typename = std::enable_if_t<!std::is_void_v<ReturnValue>>>
	SC_Future<ReturnValue> SubmitTask(const Func& aTask, const Args&...aArgs)
	{
		SC_Ref<SC_Promise<ReturnValue>> promise = SC_MakeRef<SC_Promise<ReturnValue>>();
		SC_Future<ReturnValue> future = promise->GetFuture();
		QueueTask([aTask, aArgs..., promise]
			{
				 promise->SetValue(aTask(aArgs...));
			});
		return future;
	}

private:
	SC_ThreadPool(uint32 aThreadCount);
	~SC_ThreadPool();

	bool PopTask(std::function<void()>& aOutTask);

	class WorkerThread : public SC_Thread 
	{
	public:
		WorkerThread();
		WorkerThread(const WorkerThread&) = delete;
		void SetThreadPool(SC_ThreadPool* aThreadPool);
	protected:
		void ThreadMain() override;
	private:
		SC_ThreadPool* mThreadPool;
	};

private:
	mutable SC_Mutex mQueueMutex;
	SC_Event mHasWorkEvent;

	std::atomic_bool mIsPaused;

	std::queue<SC_Function<void()>> mTasks;

	uint32 mThreadCount;

	WorkerThread* mThreads;

	std::atomic<uint32> mTotalTaskCount;

	static SC_ThreadPool* gInstance;
};

