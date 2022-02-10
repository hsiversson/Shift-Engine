#pragma once
#include <future>

template<class T>
class SC_Future
{
public:
    SC_Future() {}
    SC_Future(std::shared_future<T>&& aFuture) : mFuture(aFuture) {}
    SC_Future(SC_Future&& aOther) : mFuture(SC_Move(aOther.mFuture)) {}
    SC_Future(const SC_Future& aOther) : mFuture(aOther.mFuture) {}
    void operator=(const SC_Future& aOther) { mFuture = aOther.mFuture; }
    ~SC_Future() {}

    T GetValue() const { return mFuture.get(); }

    bool IsReady() const 
    { 
		if (!mFuture.valid())
			return true;
        return mFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready; 
    }
    bool Wait( uint32 aMilliseconds = 0 ) const
    {
        if (!mFuture.valid())
            return true;

        if (aMilliseconds)
        {
            std::future_status status = mFuture.wait_for( std::chrono::milliseconds( aMilliseconds ) );
            return status == std::future_status::ready;
        }
        else
        {
            mFuture.wait();
            return true;
        }
    }

private:
    std::shared_future<T> mFuture;
};

template<class T>
class SC_Promise
{
public:
	SC_Promise() {}
    SC_Promise(const SC_Promise&) = delete;
	void operator=(const SC_Promise&) = delete;
    ~SC_Promise() {}

	void SetValue(const T& aValue) { mPromise.set_value(aValue); }
	void SetValue(T&& aValue) { mPromise.set_value(SC_Move(aValue)); }

    SC_Future<T> GetFuture() { return SC_Future<T>(mPromise.get_future()); }

private:
    std::promise<T> mPromise;
};