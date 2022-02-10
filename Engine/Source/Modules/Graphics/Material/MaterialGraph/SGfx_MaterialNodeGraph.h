#pragma once

class SGfx_MaterialNode;
class SGfx_MaterialCompiler;
class SGfx_MaterialNodeGraph
{
public:
	SGfx_MaterialNodeGraph();
	~SGfx_MaterialNodeGraph();

	void Execute();

	bool Load();
	bool Save();

	void AddNode(const SC_Ref<SGfx_MaterialNode>& aNode);

	const SC_Array<SC_Ref<SGfx_MaterialNode>>& GetNodes() const;

private:
	void ProcessNode(SGfx_MaterialNode* aNode, SGfx_MaterialCompiler* aCompiler, SC_Array<bool>& aNodesAlreadyProcessed);
	void CreatePinConnection(SGfx_MaterialNode* aSourceNode, uint32 aInputPinId, SGfx_MaterialNode* aDestinationNode, uint32 aOutputPinId);

private:
	SGfx_MaterialNode* mRootNode;
	SC_Array<SC_Ref<SGfx_MaterialNode>> mNodes;
};

