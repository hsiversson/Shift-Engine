#include "SC_NodeGraph.h"

SC_NodeGraph::SC_NodeGraph()
	: mLatestNodeId(0)
{

}

SC_NodeGraph::~SC_NodeGraph()
{

}

void SC_NodeGraph::AddNode(const SC_Ref<SC_Node>& aNode)
{
	SC_ASSERT(aNode, "Null node passed into AddNode.");

	uint32 id = ++mLatestNodeId;
	for (;;)
	{
		if (mAddedNodeIds.Find(id) != mAddedNodeIds.gFindResultNone)
			++id;
		else
			break;
	}

	aNode->mParentGraph = this;
	aNode->mId = id;
	mNodes.Add(aNode);
	mAddedNodeIds.Add(id);

	// OnChanged(ChangeFlag_AddNode);
}

void SC_NodeGraph::RemoveNode(const SC_Ref<SC_Node>& aNode)
{
	SC_ASSERT(aNode, "Null node passed into RemoveNode.");
	SC_ASSERT(aNode->mParentGraph == this, "This node does not belong to this graph");

	// Disconnect all pins

	uint32 id = aNode->mId;
	mAddedNodeIds.Remove(id);

	uint32 index = 0;
	for (SC_Ref<SC_Node>& node : mNodes)
	{
		if (node->mId == id)
		{
			mNodes.RemoveAt(index);
			break;
		}
		++index;
	}

	if (mRootNode == aNode)
		mRootNode.Reset();

	aNode->mParentGraph = nullptr;
	aNode->mId = SC_UINT32_MAX;
}

bool SC_NodeGraph::HasNode(const SC_Ref<SC_Node>& aNode) const
{
	SC_ASSERT(aNode, "Null node passed into HasNode.");
	return aNode->mParentGraph == this;
}

void SC_NodeGraph::Execute()
{
	mRootNode->Execute();
}

SC_NodeGraph::Nodes& SC_NodeGraph::GetNodes()
{
	return mNodes;
}

const SC_NodeGraph::Nodes& SC_NodeGraph::GetNodes() const
{
	return mNodes;
}

SC_Node* SC_NodeGraph::GetRootNode() const
{
	return mRootNode;
}
