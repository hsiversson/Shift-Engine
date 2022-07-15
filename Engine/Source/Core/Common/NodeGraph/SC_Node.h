#pragma once
#include "SC_NodePin.h"

class SC_NodeGraph;
class SC_Node
{
	friend class SC_NodeGraph;
public:
	SC_Node();
	~SC_Node();

	virtual bool IsConstant() { return true; }

	virtual void Load();		// Runs once when we initially load the node into a nodegraph template.
	virtual void Prepare();		// Runs once for each nodegraph instance
	virtual void Execute();		// Runs continously if required.

	SC_Array<SC_NodePin>& GetPins();
	const SC_Array<SC_NodePin>& GetPins() const;
	uint32 GetPinCount() const;

	SC_NodePin* GetPin(uint32 aPinIndex);
	const SC_NodePin* GetPin(uint32 aPinIndex) const;

	SC_NodeGraph* GetGraph() const;
	uint32 GetId() const;
protected:

private:
	SC_Array<SC_NodePin> mPins;

	SC_NodeGraph* mParentGraph;
	uint32 mId;
};

