#pragma once

struct SGfx_ViewData;
class SR_CommandList;
class SGfx_DebugRenderer
{
public:
	SGfx_DebugRenderer();
	~SGfx_DebugRenderer();

	bool Init();

	void SetDrawGrid(bool aValue);
	bool GetDrawGrid();

	void Render(SR_CommandList* aCmdList, const SGfx_ViewData& aRenderData);

private:
	SC_Ref<SR_BufferResource> mVertexBuffer;
	SC_Ref<SR_ShaderState> mLineShader;

	bool mDrawGrid;
};

