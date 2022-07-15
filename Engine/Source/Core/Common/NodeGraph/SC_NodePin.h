#pragma once

class SC_Node;
struct SC_NodePin;

struct SC_NodeConnection
{
	SC_NodePin* mSource;
	SC_NodePin* mDestination;

	SC_NodePin* GetOther(const SC_NodePin* aSelf) const
	{
		SC_ASSERT(aSelf == mSource || aSelf == mDestination);
		return aSelf == mSource ? mDestination : mSource;
	}
};

struct SC_NodePin
{
	static constexpr uint8 gMaxPinCount = 127;
	static constexpr uint8 gMaxConnectionCount = 127;

	SC_Array<SC_NodeConnection> mConnections;

	SC_Node* mParentNode;

	uint16 mId;
	uint16 mIndex;
	bool mIsNodeInput;
	bool mIsOptional;
};

