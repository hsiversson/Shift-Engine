#pragma once
#include <queue>

template<class T>
using SC_Queue = std::queue<T>;

template<class T>
using SC_Deque = std::deque<T>;

template<class T>
class SC_LocklessQueue
{
public:
	SC_LocklessQueue() {}
	~SC_LocklessQueue() {}
};