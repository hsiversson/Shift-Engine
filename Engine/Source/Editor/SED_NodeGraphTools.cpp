#include "SED_NodeGraphTools.h"
#include "RenderCore/Interface/SR_RenderTypes.h"

namespace SED_NodeGraphTools
{
	struct SED_NodeGraphEditorContext
	{
		SR_Rect mViewRect;
		float mViewScale;
		float mViewScaleRcp;

		SC_IntVector2 mMouseStartDragPos;
		SC_IntVector2 mCanvasViewOffset;

		bool mCanvasVisible;
	};
	static SED_NodeGraphEditorContext* gEditorContext = nullptr;


	static inline ImVec2 SelectPositive(const ImVec2& aLeft, const ImVec2& aRight) 
	{ 
		return ImVec2(aLeft.x > 0.0f ? aLeft.x : aRight.x, aLeft.y > 0.0f ? aLeft.y : aRight.y); 
	}

	static bool BeginCanvas(uint32 aId, const SC_Vector2& aSize)
	{
		const ImVec2 inSize = aSize;
	
		ImVec2 position = ImGui::GetCursorScreenPos();
		ImVec2 size = SelectPositive(inSize, ImGui::GetContentRegionAvail());
		ImRect rect = ImRect(position, position + size);
		ImDrawList* drawList = ImGui::GetWindowDrawList();
	
		if (ImGui::IsClippedEx(rect, ImGuiID(aId), false))
			return false;
	
		drawList->AddRect(rect.Min, rect.Max, IM_COL32(255, 0, 255, 64));
		ImGui::SetCursorScreenPos(ImVec2(0.0f, 0.0f));
	
		gEditorContext->mViewRect = SR_Rect({ (uint32)rect.Min.x, (uint32)rect.Min.y, (uint32)rect.Max.x, (uint32)rect.Max.y });

		if (ImGui::IsWindowFocused() && ImGui::IsMouseHoveringRect(rect.Min, rect.Max))
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				gEditorContext->mMouseStartDragPos = SC_Vector2(ImGui::GetMousePos());

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				gEditorContext->mCanvasViewOffset = SC_IntVector2(0);

			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				gEditorContext->mCanvasViewOffset = gEditorContext->mMouseStartDragPos + SC_Vector2(ImGui::GetMouseDragDelta(ImGuiMouseButton_Left));
			}
		}

		return false;
	}

	static bool BeginCanvas(const char* aId, const SC_Vector2& aSize)
	{
		return BeginCanvas(ImGui::GetID(aId), aSize);
	}
	
	static void EndCanvas()
	{
	}

	bool CreateContext()
	{
		if (!gEditorContext)
		{
			gEditorContext = new SED_NodeGraphEditorContext;
			return true;
		}

		SC_ASSERT(false, "Context Already Created!");
		return false;
	}

	void DestroyContext()
	{
		delete gEditorContext;
		gEditorContext = nullptr;
	}

	bool BeginEditor(const char* aId, const SC_Vector2& aSize)
	{
		ImGui::PushID(aId);

		auto availableContentSize = ImGui::GetContentRegionAvail();
		ImVec2 canvasSize = ImFloor(aSize);
		if (canvasSize.x <= 0.0f)
			canvasSize.x = ImMax(4.0f, availableContentSize.x);
		if (canvasSize.y <= 0.0f)
			canvasSize.y = ImMax(4.0f, availableContentSize.y);

		gEditorContext->mCanvasVisible = BeginCanvas(aId, canvasSize);

		return true;
	}

	void EndEditor()
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();


		// Draw background grid
		{
			float gridSizeX = 32.0f;// * m_Canvas.ViewScale();
			float gridSizeY = 32.0f;// * m_Canvas.ViewScale();
			ImVec2 viewPos = gEditorContext->mViewRect.TopLeft();
			ImVec2 viewSize = gEditorContext->mViewRect.Size();
			ImVec2 offset = gEditorContext->mCanvasViewOffset + (gEditorContext->mViewRect.Size() / 2);

			drawList->AddRectFilled(viewPos, viewPos + viewSize, ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_WindowBg]));

			for (float x = fmodf(offset.x, gridSizeX); x < viewSize.x; x += gridSizeX)
				drawList->AddLine(ImVec2(x, 0.0f) + viewPos, ImVec2(x, viewSize.y) + viewPos, ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_FrameBg]));
			for (float y = fmodf(offset.y, gridSizeY); y < viewSize.y; y += gridSizeY)
				drawList->AddLine(ImVec2(0.0f, y) + viewPos, ImVec2(viewSize.x, y) + viewPos, ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_FrameBg]));
		}

		if (gEditorContext->mCanvasVisible)
			EndCanvas();

		ImGui::PopID();
	}

	bool BeginNode()
	{


		return false;
	}

	void EndNode()
	{
	}

	bool BeginPin()
	{
		return false;
	}

	void EndPin()
	{
	}
}