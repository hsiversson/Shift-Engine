#pragma once
#include "SC_Node.h"

class SC_NodeGraph
{
public:
	using Nodes = SC_Array<SC_Ref<SC_Node>>;

public:
	SC_NodeGraph();
	virtual ~SC_NodeGraph();

	void AddNode(const SC_Ref<SC_Node>& aNode);
	void RemoveNode(const SC_Ref<SC_Node>& aNode);
	bool HasNode(const SC_Ref<SC_Node>& aNode) const;

	void Execute();

	Nodes& GetNodes();
	const Nodes& GetNodes() const;
	SC_Node* GetRootNode() const; 

private:
	Nodes mNodes;
	SC_Ref<SC_Node> mRootNode;

	SC_Array<uint32> mAddedNodeIds;
	uint32 mLatestNodeId;
};

