#include "SC_Node.h"

SC_Node::SC_Node()
{

}

SC_Node::~SC_Node()
{

}

void SC_Node::Load()
{

}

void SC_Node::Prepare()
{

}

void SC_Node::Execute()
{

}

SC_Array<SC_NodePin>& SC_Node::GetPins()
{
	return mPins;
}

const SC_Array<SC_NodePin>& SC_Node::GetPins() const
{
	return mPins;
}

uint32 SC_Node::GetPinCount() const
{
	return mPins.Count();
}

SC_NodePin* SC_Node::GetPin(uint32 aPinIndex)
{
	return &mPins[aPinIndex];
}

const SC_NodePin* SC_Node::GetPin(uint32 aPinIndex) const
{
	return &mPins[aPinIndex];
}

SC_NodeGraph* SC_Node::GetGraph() const
{
	return mParentGraph;
}

uint32 SC_Node::GetId() const
{
	return mId;
}
