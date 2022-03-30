#pragma once

#define ENABLE_DEBUG_DRAW (1)

struct SGfx_ViewData;
class SR_CommandList;
class SGfx_DebugRenderer
{
public:
	SGfx_DebugRenderer();
	~SGfx_DebugRenderer();

	static SGfx_DebugRenderer* Get();

	bool Init();

	void SetDrawGrid(bool aValue);
	bool GetDrawGrid();

	void DrawLine(const SC_Vector& aFrom, const SC_Vector& aTo, const SC_Vector& aFromColor = SC_Vector(1), const SC_Vector& aToColor = SC_Vector(1));
	void DrawLineNoDepthTest(const SC_Vector& aFrom, const SC_Vector& aTo, const SC_Vector& aFromColor = SC_Vector(1), const SC_Vector& aToColor = SC_Vector(1));

	void Render(SR_CommandList* aCmdList, const SGfx_ViewData& aRenderData);

private:
	SC_Ref<SR_BufferResource> mVertexBuffer;
	SC_Ref<SR_BufferResource> mLinesBuffer;
	SC_Ref<SR_ShaderState> mLineShader;

	struct Vert
	{
		SC_Vector mPosition;
		SC_Vector4 mColor;
	};
	struct Line
	{
		Vert mFrom;
		Vert mTo;
	};
	SC_Array<Line> mLines;
	SC_Mutex mLinesMutex;

	bool mDrawGrid;

	static SGfx_DebugRenderer* gInstance;
};

#define DRAW_LINE_COLORED(aFrom, aTo, aColor)								SGfx_DebugRenderer::Get()->DrawLine(aFrom, aTo, aColor, aColor)
#define DRAW_LINE_COLORED2(aFrom, aTo, aFromColor, aToColor)				SGfx_DebugRenderer::Get()->DrawLine(aFrom, aTo, aFromColor, aToColor)
#define DRAW_LINE_COLORED_NO_DEPTHTEST(aFrom, aTo, aColor)
#define DRAW_LINE_COLORED2_NO_DEPTHTEST(aFrom, aTo, aFromColor, aToColor)