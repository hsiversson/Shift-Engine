#include "SGfx_MaterialNode.h"

bool SGfx_MaterialNode::AddConnection(SGfx_MaterialNodeOutputPin* aSourcePin, uint32 aInputPinIndex)
{
    SGfx_MaterialNodeInputPin& inPin = mInputs[aInputPinIndex];
    inPin.mConnection.mSource = aSourcePin;
    inPin.mConnection.mDestination = &inPin;

    aSourcePin->mConnections.AddUnique(inPin.mConnection);
    return true;
}

bool SGfx_MaterialNode::RemoveConnection(SGfx_MaterialNodeOutputPin* /*aSourcePin*/, uint32 aInputPinIndex)
{
    mInputs[aInputPinIndex].mConnection.mSource = nullptr;
	return true;
}

bool SGfx_MaterialNode::AddConnection(SGfx_MaterialNodeInputPin* aDestinationPin, uint32 aOutputPinIndex)
{
    SGfx_MaterialNodeOutputPin& outPin = mOutputs[aOutputPinIndex];

    if (!outPin.mConnections.AddUnique(SGfx_MaterialNodeConnection(&outPin, aDestinationPin)))
    {
        SC_ASSERT(false, "Pin connection already present.");
        return false;
    }

    return true;
}

bool SGfx_MaterialNode::RemoveConnection(SGfx_MaterialNodeInputPin* aDestinationPin, uint32 aOutputPinIndex)
{
	SGfx_MaterialNodeOutputPin& outPin = mOutputs[aOutputPinIndex];
    outPin.mConnections.RemoveCyclic(SGfx_MaterialNodeConnection(&outPin, aDestinationPin));
    return true;
}
