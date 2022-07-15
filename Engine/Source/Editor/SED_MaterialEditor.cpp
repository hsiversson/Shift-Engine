#include "SED_MaterialEditor.h"
#include "SED_ViewportWindow.h"
#include "SED_NodeGraphTools.h"

#include "Graphics/World/SGfx_World.h"
#include "Graphics/Material/MaterialGraph/SGfx_MaterialNodeGraph.h"
#include "Graphics/Material/MaterialGraph/SGfx_MaterialNode.h"


SED_MaterialEditor::SED_MaterialEditor()
    : mIsOpen(true)
{
    SED_NodeGraphTools::CreateContext();
}

SED_MaterialEditor::~SED_MaterialEditor()
{
	SED_NodeGraphTools::DestroyContext();
}

bool SED_MaterialEditor::Init()
{
    mGfxWorld = SC_MakeUnique<SGfx_World>();
    mViewport = SC_MakeUnique<SED_ViewportWindow>(mGfxWorld.get(), "Material Editor Viewport");

    return true;
}

void SED_MaterialEditor::OnUpdate()
{
    if (mIsOpen)
        mViewport->Update();
}

void SED_MaterialEditor::OnRender()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_None;

    ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Material Editor", &mIsOpen, flags))
    {
        SED_NodeGraphTools::BeginEditor("graph"/*mCurrentGraph->GetName()*/, ImGui::GetContentRegionAvail());

        const SC_Array<SC_Ref<SGfx_MaterialNode>>& nodes = mCurrentGraph->GetNodes();
        for (const SC_Ref<SGfx_MaterialNode>& node : nodes)
        {
            SED_NodeGraphTools::BeginNode();

            // Draw node title bar (eg. name, type...)

            const SC_Array<SGfx_MaterialNodeInputPin>& nodeInputs = node->GetInputs();
            for (const SGfx_MaterialNodeInputPin& pin : nodeInputs)
            {
                SED_NodeGraphTools::BeginPin(); // IsInput=true
                (void)pin;
                SED_NodeGraphTools::EndPin();
            }

            const SC_Array<SGfx_MaterialNodeOutputPin>& nodeOutputs = node->GetOutputs();
			for (const SGfx_MaterialNodeOutputPin& pin : nodeOutputs)
			{
				SED_NodeGraphTools::BeginPin(); // IsInput=false
				(void)pin;
				SED_NodeGraphTools::EndPin();
			}

			SED_NodeGraphTools::EndNode();
        }

        // Draw connections

		SED_NodeGraphTools::EndEditor();
		mViewport->Draw();
    }
	ImGui::End();
	//if (ImGui::Begin("Material Properties"))
	//{
	//    float v[3] = {};
	//	ImGui::DragFloat3("Test", v);
	//}
	//ImGui::End();

}
