#include "SGfx_MaterialNodeGraph.h"
#include "SGfx_MaterialNode.h"
#include "SGfx_MaterialNode_Result.h"
#include "SGfx_MaterialCompiler.h"

#include "SGfx_MaterialNodes.h"
#include "SGfx_MaterialNode_TextureSample.h"

SGfx_MaterialNodeGraph::SGfx_MaterialNodeGraph()
{

}

SGfx_MaterialNodeGraph::~SGfx_MaterialNodeGraph()
{

}

void SGfx_MaterialNodeGraph::Execute()
{

	uint32 nodeId = 0;

	mRootNode = new SGfx_MaterialNode_Result();
	mRootNode->mNodeId = nodeId++;

	SGfx_MaterialNode_Constant c1;
	c1.mNodeId = nodeId++;
	c1.SetValue(SC_Vector4(1.0f));

	SGfx_MaterialNode_Constant c2;
	c2.mNodeId = nodeId++;
	c2.SetValue(SC_Vector4(2.0f));

	SGfx_MaterialNode_Constant c3;
	c3.mNodeId = nodeId++;
	c3.SetValue(SC_Vector4(3.0f));

	SGfx_MaterialNode_Constant c4;
	c4.mNodeId = nodeId++;
	c4.SetValue(1.0f);

	SGfx_MaterialNode_Add a1;
	a1.mNodeId = nodeId++;
	CreatePinConnection(&c1, SGfx_MaterialNode_Constant::Out, &a1, SGfx_MaterialNode_Add::In1);
	CreatePinConnection(&c2, SGfx_MaterialNode_Constant::Out, &a1, SGfx_MaterialNode_Add::In2);

	SGfx_MaterialNode_Subtract s1;
	s1.mNodeId = nodeId++;
	CreatePinConnection(&a1, SGfx_MaterialNode_Add::Out, &s1, SGfx_MaterialNode_Subtract::In1);
	CreatePinConnection(&c2, SGfx_MaterialNode_Constant::Out, &s1, SGfx_MaterialNode_Subtract::In2);

	SGfx_MaterialNode_Multiply m1;
	m1.mNodeId = nodeId++;
	CreatePinConnection(&a1, SGfx_MaterialNode_Add::Out, &m1, SGfx_MaterialNode_Multiply::In1);
	CreatePinConnection(&s1, SGfx_MaterialNode_Subtract::Out, &m1, SGfx_MaterialNode_Multiply::In2);

	SGfx_MaterialNode_Divide d1;
	d1.mNodeId = nodeId++;
	CreatePinConnection(&c3, SGfx_MaterialNode_Constant::Out, &m1, SGfx_MaterialNode_Divide::In1);
	CreatePinConnection(&a1, SGfx_MaterialNode_Add::Out, &m1, SGfx_MaterialNode_Divide::In2);

	SGfx_MaterialNode_TextureSample ts1;
	ts1.mNodeId = nodeId++;
	CreatePinConnection(&c3, SGfx_MaterialNode_Constant::Out, &ts1, SGfx_MaterialNode_TextureSample::InTexture);
	CreatePinConnection(&a1, SGfx_MaterialNode_Add::Out, &ts1, SGfx_MaterialNode_TextureSample::InSampler);
	CreatePinConnection(&a1, SGfx_MaterialNode_Add::Out, &ts1, SGfx_MaterialNode_TextureSample::InUV);

	CreatePinConnection(&a1, SGfx_MaterialNode_Add::Out, mRootNode, SGfx_MaterialNode_Result::BaseColor);
	CreatePinConnection(&a1, SGfx_MaterialNode_Multiply::Out, mRootNode, SGfx_MaterialNode_Result::Normal);
	CreatePinConnection(&ts1, SGfx_MaterialNode_TextureSample::OutR, mRootNode, SGfx_MaterialNode_Result::Roughness);
	CreatePinConnection(&ts1, SGfx_MaterialNode_TextureSample::OutG, mRootNode, SGfx_MaterialNode_Result::Metallic);
	CreatePinConnection(&ts1, SGfx_MaterialNode_TextureSample::OutB, mRootNode, SGfx_MaterialNode_Result::AmbientOcclusion);
	CreatePinConnection(&ts1, SGfx_MaterialNode_TextureSample::OutA, mRootNode, SGfx_MaterialNode_Result::Specular);

	SGfx_MaterialCompiler compiler;
	SC_Array<bool> nodesAlreadyProcessed;
	nodesAlreadyProcessed.Respace(nodeId);
	SC_ZeroMemory(nodesAlreadyProcessed.GetBuffer(), nodesAlreadyProcessed.GetByteSize());
	ProcessNode(mRootNode, &compiler, nodesAlreadyProcessed);
}

bool SGfx_MaterialNodeGraph::Load()
{
	return false;
}

bool SGfx_MaterialNodeGraph::Save()
{
	return false;
}

void SGfx_MaterialNodeGraph::AddNode(const SC_Ref<SGfx_MaterialNode>& aNode)
{
	//aNode->mNodeId = GetFreeNodeId();
	mNodes.Add(aNode);
}

const SC_Array<SC_Ref<SGfx_MaterialNode>>& SGfx_MaterialNodeGraph::GetNodes() const
{
	return mNodes;
}

void SGfx_MaterialNodeGraph::ProcessNode(SGfx_MaterialNode* aNode, SGfx_MaterialCompiler* aCompiler, SC_Array<bool>& aNodesAlreadyProcessed)
{
	const SC_Array<SGfx_MaterialNodeInputPin>& inputPins = aNode->GetInputs();
	for (const SGfx_MaterialNodeInputPin& pin : inputPins)
	{
		if (pin.mConnection.mSource)
		{
			const uint32 nodeId = pin.mConnection.mSource->GetParent()->GetId();
			if (!aNodesAlreadyProcessed[nodeId])
			{
				ProcessNode(pin.mConnection.mSource->GetParent(), aCompiler, aNodesAlreadyProcessed);
				aNodesAlreadyProcessed[nodeId] = true;
			}
		}
	}

	aNode->Compile(aCompiler);
}

void SGfx_MaterialNodeGraph::CreatePinConnection(SGfx_MaterialNode* aSourceNode, uint32 aOutputPinId, SGfx_MaterialNode* aDestinationNode, uint32 aInputPinId)
{
	SGfx_MaterialNodeOutputPin* outputPin = aSourceNode->GetOutputPin(aOutputPinId);
	SGfx_MaterialNodeInputPin* inputPin = aDestinationNode->GetInputPin(aInputPinId);

	inputPin->mConnection.mDestination = inputPin;
	inputPin->mConnection.mSource = outputPin;

	outputPin->mConnections.AddUnique(inputPin->mConnection);
}
