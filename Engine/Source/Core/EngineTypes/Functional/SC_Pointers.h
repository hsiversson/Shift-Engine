#pragma once
#include <memory>
#include "SC_Ref.h"
#include "SC_Function.h"

//template<class T>
//using SC_Ref = std::shared_ptr<T>;

template<class T, class... Args>
inline SC_Ref<T> SC_MakeRef(Args&&... _Args)
{
	return new T(std::forward<Args>(_Args)...);
}

template<class T>
using SC_UniquePtr = std::unique_ptr<T>;

template<class T, class... Args>
inline SC_UniquePtr<T> SC_MakeUnique(Args&&... _Args)
{
	return std::make_unique<T>(std::forward<Args>(_Args)...);
}