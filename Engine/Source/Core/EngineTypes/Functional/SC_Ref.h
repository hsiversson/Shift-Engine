#pragma once
#include "Platform/Platform/SC_Platform.h"
#include "Platform/Atomics/SC_Atomics.h"
#include "EngineTypes/Containers/SC_Pair.h"
#include <memory>

class SC_NOVTABLE SC_RefCountedBase
{
protected:
	constexpr SC_RefCountedBase() noexcept = default;
public:
	SC_RefCountedBase(const SC_RefCountedBase&) = delete;
	SC_RefCountedBase& operator=(const SC_RefCountedBase&) = delete;
	virtual ~SC_RefCountedBase() noexcept {}

	bool Increment_NotZero();

	void Increment();
	void Decrement();

	void IncrementWeak();
	void DecrementWeak();

	uint32 GetCount() const;

	virtual void* GetDeleter(const type_info&) const noexcept;
private:
	virtual void Destroy() noexcept = 0;
	virtual void DestroySelf() noexcept = 0;

private:
	volatile uint32 mCounter = 1;
	volatile uint32 mWeakCounter = 1;
};

template<class T>
class SC_RefCounted : public SC_RefCountedBase
{
public:
	explicit SC_RefCounted(T* aPtr) : SC_RefCounted(), mPtr(aPtr) {}
private:
	void Destroy() noexcept override { delete mPtr; }
	void DestroySelf() noexcept override { delete this; }

	T* mPtr;
};

template<class ResourceType, class DeleterType>
class SC_RefCountedResource : public SC_RefCountedBase
{
public:
	SC_RefCountedResource(ResourceType aPtr, DeleterType aDeleter)
		: SC_RefCountedBase()
		, mPair(SC_Move(aDeleter), aPtr) 
	{}

	~SC_RefCountedResource() noexcept override {}

	void* GetDeleter(const type_info& aTypeId) const noexcept override
	{
#if ENABLE_RTTI
		if (aTypeId == typeid(DeleterType))
			return const_cast<DeleterType*>(std::addressof(mPair.mFirst));
#else // ENABLE_RTTI
		(void)aTypeId;
#endif // ENABLE_RTTI

		return nullptr;
	}
private:
	void Destroy() noexcept override { mPair.mFirst(mPair.mSecond); }
	void DestroySelf() noexcept override { delete this; }

	SC_Pair<DeleterType, ResourceType> mPair;
};

// template<class ResourceType, class DeleterType, class AllocType>
// class SC_RefCountedResourceAlloc : public SC_RefCountedBase
// {
// public:
// 	SC_RefCountedResource(ResourceType aPtr, DeleterType aDeleter, const AllocType& aAllocator)
// 		: SC_RefCountedBase()
// 		, mPair(SC_Move(aDeleter), SC_Pair(aAllocator, aPtr))
// 	{}
// 
// 	~SC_RefCountedResource() noexcept override {}
// 
// 	void* GetDeleter(const type_info& aTypeId) const noexcept override
// 	{
// #if ENABLE_RTTI
// 		if (aTypeId == typeid(DeleterType))
// 			return const_cast<DeleterType*>(std::addressof(mPair.mFirst));
// #else // ENABLE_RTTI
// 		(void)aTypeId;
// #endif // ENABLE_RTTI
// 
// 		return nullptr;
// 	}
// private:
// 	using ReboundAlloc = _Rebind_alloc_t<AllocType, SC_RefCountedResourceAlloc>;
// 	void Destroy() noexcept override { mPair.mFirst(mPair.mSecond.mSecond); }
// 	void DestroySelf() noexcept override 
// 	{ 
// 		ReboundAlloc alloc = mPair.mSecond.mFirst();
// 		this->~SC_RefCountedResourceAlloc();
// 		_Deallocate_plain(alloc, this);
// 	}
// 
// 	SC_Pair<DeleterType, SC_Pair<ReboundAlloc, ResourceType>> mPair;
// };

template<class T> class SC_Ref;
template<class T> class SC_WeakPtr;
template<class T>
class SC_PointerBase
{
	template <class T1> friend class SC_PointerBase;
	friend SC_Ref<T>;
public:
	using PointerType = std::remove_extent_t<T>;

	SC_PointerBase(const SC_PointerBase&) = delete;
	SC_PointerBase& operator=(const SC_PointerBase&) = delete;

	uint32 GetRefCount() const { return (mControlBlock) ? mControlBlock->GetCount() : 0; }

protected:
	constexpr SC_PointerBase() noexcept = default;
	~SC_PointerBase() = default;

	PointerType* Get() const noexcept { return mPtr; }

	void AddRef() const noexcept 
	{
		if (mControlBlock)
			mControlBlock->Increment();
	}

	void RemoveRef() const noexcept 
	{
		if (mControlBlock)
			mControlBlock->Decrement();
	}

	void AddWeakRef() const noexcept
	{
		if (mControlBlock)
			mControlBlock->IncrementWeak();
	}

	void RemoveWeakRef() const noexcept
	{
		if (mControlBlock)
			mControlBlock->DecrementWeak();
	}

private:
#if ENABLE_RTTI
	template <class DeleterType, class T1>
	friend DeleterType* GetDeleter(const SC_Ref<T1>& aRef) noexcept;
#endif

private:
	PointerType* mPtr{ nullptr };
	SC_RefCountedBase* mControlBlock{ nullptr };
};

template<typename T>
class SC_Ref2 : public SC_PointerBase<T>
{
private:
	using BaseClass = SC_PointerBase<T>;
public:
	using typename BaseClass::PointerType;

	constexpr SC_Ref2() noexcept = default;
	constexpr SC_Ref2(SC_Nullptr) noexcept {}

	~SC_Ref2() noexcept { this->RemoveRef(); }

	explicit operator bool() const noexcept { return this->Get() != nullptr; }

protected:

private:
};

template<typename>
class SC_WeakPtr
{

};

// TEMPORARILY JUST A WRAPPER AROUND STD::SHARED_PTR
template <class T>
class SC_Ref
{
	template <class Y> friend class SC_Ref;
public:
	SC_Ref() {}
	SC_Ref(SC_Nullptr) {}
	SC_Ref(T* aData) : mPtr(std::shared_ptr<T>(aData)) {}
	SC_Ref(std::shared_ptr<T>& aSharedPtr) : mPtr(aSharedPtr) {}
	~SC_Ref() {}

	void Reset(T* aData = nullptr)
	{
		mPtr.reset(aData);
	}

	T* Get() const { return mPtr.get(); }
	operator T*() const { return mPtr.get(); }

	uint32 GetRefCount() const { return (uint32)mPtr.use_count(); }

	T* operator->() const { return mPtr.get(); }
	T& operator*() const { return *(mPtr.get()); }

	bool operator!() const { return mPtr == nullptr; }

	bool operator==(const SC_Ref<T>& aOther) const { return mPtr == aOther.mPtr; }
	bool operator!=(const SC_Ref<T>& aOther) const { return mPtr != aOther.mPtr; }
	bool operator==(const T* aOther) const { return mPtr.get() == aOther; }
	bool operator!=(const T* aOther) const { return mPtr.get() != aOther; }

	template <typename Y>
	SC_Ref(const SC_Ref<Y>& aOther) : mPtr(aOther.mPtr) {}

	template <typename Y>
	SC_Ref& operator=(const SC_Ref<Y>& aOther) { mPtr = aOther.mPtr; return *this; }

private:
	std::shared_ptr<T> mPtr;
};
