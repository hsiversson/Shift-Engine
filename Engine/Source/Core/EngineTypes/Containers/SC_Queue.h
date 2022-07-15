#pragma once
#include <queue>

template<class T>
using SC_Queue = std::queue<T>;

template<class T>
using SC_Deque = std::deque<T>;

template<class T>
class SC_AtomicQueue
{
public:
	SC_AtomicQueue() {}
	~SC_AtomicQueue() {}
};