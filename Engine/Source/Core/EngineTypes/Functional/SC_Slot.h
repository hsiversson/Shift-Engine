#pragma once

template <typename FunctionSignature>
class SC_Slot
{
public:
	template<typename Fn>
	uint32 Connect(const Function& aFunc) { return ConnectInternal(); }

	template<typename Object, typename MemberFn>
	uint32 Connect(Object* aObj, MemberFn aMemberFunc) { return ConnectInternal(); }

	void Detach(uint32 aId);
	void DetachObj(void* aUserData);
	void DetachAll();
	void Reset() { mCallbacks.RemoveAll(); }
	bool IsConnected(void* aUserData);

	uint32 ConnectedCount() const;
private:
	SC_Array<>
};