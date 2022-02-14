#include "SGfx_DebugRenderer.h"
#include "RenderCore/Interface/SR_RenderDevice.h"

static const char* GetVertexShaderCode()
{
	return
		"#include \"Common.ssh\"\n"
		"#include \"SceneConstants.ssh\"\n"
		"struct InputStruct\n"
		"{\n"
		"	float4 mPosition : POSITION;\n"
		"	float4 mColor : COLOR0;\n"
		"};\n"
		"struct OutputStruct\n"
		"{\n"
		"	float4 mPosition : SV_POSITION;\n"
		"	float4 mColor : COLOR0; \n"
		"};\n"
		"OutputStruct main(InputStruct aInput)\n"
		"{\n"
		"	OutputStruct output;\n"
		"	output.mPosition = mul(gSceneConstants.mViewConstants.mWorldToClip_NoJitter, float4(aInput.mPosition.xyz, 1.f));\n"
		"	output.mColor = aInput.mColor;\n"
		"	return output;\n"
		"}\n";
}

static const char* GetPixelShaderCode()
{
	return
		"#include \"Common.ssh\"\n"
		"#include \"SceneConstants.ssh\"\n"
		"struct InputStruct\n"
		"{\n"
		"	float4 mPosition : SV_POSITION;\n"
		"	float4 mColor : COLOR0;\n"
		"};\n"
		"struct OutputStruct\n"
		"{\n"
		"	float4 mColor : SV_TARGET0;\n"
		"};\n"
		"OutputStruct main(InputStruct aInput)\n"
		"{\n"
		"	OutputStruct output;\n"
		"	output.mColor = aInput.mColor;\n"
		"	return output;\n"
		"}\n";
}

SGfx_DebugRenderer::SGfx_DebugRenderer()
	: mDrawGrid(true)
{

}

SGfx_DebugRenderer::~SGfx_DebugRenderer()
{

}

bool SGfx_DebugRenderer::Init()
{
	SR_ShaderCompileArgs compileArgs;
	compileArgs.mEntryPoint = "main";
	compileArgs.mType = SR_ShaderType::Vertex;

	SR_ShaderStateProperties lineShaderProps;
	if (!SR_RenderDevice::gInstance->CompileShader(GetVertexShaderCode(), compileArgs, lineShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Vertex)], &lineShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Vertex)]))
		return false;

	compileArgs.mType = SR_ShaderType::Pixel;
	if (!SR_RenderDevice::gInstance->CompileShader(GetPixelShaderCode(), compileArgs, lineShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Pixel)], &lineShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Pixel)]))
		return false;

	lineShaderProps.mVertexLayout.SetAttribute(SR_VertexAttribute::Position, SR_Format::RGBA32_FLOAT);
	lineShaderProps.mVertexLayout.SetAttribute(SR_VertexAttribute::Color0, SR_Format::RGBA32_FLOAT);
	lineShaderProps.mPrimitiveTopology = SR_PrimitiveTopology::LineList;

	lineShaderProps.mRasterizerProperties.mCullMode = SR_CullMode::None;
	lineShaderProps.mBlendStateProperties.mNumRenderTargets = 1;
	lineShaderProps.mRTVFormats.mNumColorFormats = 1;
	lineShaderProps.mRTVFormats.mColorFormats[0] = SR_Format::RGBA8_UNORM;
	lineShaderProps.mDepthStencilProperties.mWriteDepth = false;
	lineShaderProps.mDepthStencilProperties.mDepthComparisonFunc = SR_ComparisonFunc::GreaterEqual;
	mLineShader = SR_RenderDevice::gInstance->CreateShaderState(lineShaderProps);

	struct Vert
	{
		SC_Vector mPosition;
		SC_Vector4 mColor;
	};
	SC_Array<Vert> vertices;

	SC_Vector offset(-5.0f, 0.0f, -5.0f);
	for (int32 i = 0; i < 11; i++)
	{
		Vert vert = {};
		vert.mPosition = SC_Vector4((SC_Vector(1.0f, 0.0f, 0.0f) * float(i)) + offset, 1.0f);
		vert.mColor = SC_Vector4(1.0f, 1.0f, 1.0f, 1.0f); 
		vertices.Add(vert);

		vert.mPosition = SC_Vector4((SC_Vector(1.0f, 0.0f, 0.0f) * float(i)) + offset + SC_Vector(0.0f, 0.0f, 10.0f), 1.0f);
		vertices.Add(vert);

	}
	
	for (int32 i = 0; i < 11; i++)
	{
		Vert vert = {};
		vert.mPosition = SC_Vector4((SC_Vector(0.0f, 0.0f, 1.0f) * float(i)) + offset, 1.0f);
		vert.mColor = SC_Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices.Add(vert);

		vert.mPosition = SC_Vector4((SC_Vector(0.0f, 0.0f, 1.0f) * float(i)) + offset + SC_Vector(10.0f, 0.0f, 0.0f), 1.0f);
		vertices.Add(vert);
	}

	SR_BufferResourceProperties vertexBufferResourceProps;
	vertexBufferResourceProps.mElementCount = vertices.Count();
	vertexBufferResourceProps.mElementSize = vertices.ElementStride();
	vertexBufferResourceProps.mBindFlags = SR_BufferBindFlag_VertexBuffer;
	mVertexBuffer = SR_RenderDevice::gInstance->CreateBufferResource(vertexBufferResourceProps, vertices.GetBuffer());

	return true;
}

void SGfx_DebugRenderer::SetDrawGrid(bool aValue)
{
	mDrawGrid = aValue;
}

bool SGfx_DebugRenderer::GetDrawGrid()
{
	return mDrawGrid;
}

void SGfx_DebugRenderer::Render(SR_CommandList* aCmdList, const SGfx_ViewData& /*aRenderData*/)
{
	if (mDrawGrid)
	{
		aCmdList->SetVertexBuffer(mVertexBuffer.get());
		aCmdList->SetShaderState(mLineShader.get());
		aCmdList->SetPrimitiveTopology(SR_PrimitiveTopology::LineList);
		aCmdList->Draw(mVertexBuffer->GetProperties().mElementCount);
	}
}
