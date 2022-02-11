#include "SGfx_Environment.h"
#include "Graphics/Misc/SGfx_Shapes.h"
#include "RenderCore/Interface/SR_CommandList.h"

SGfx_Skybox::SGfx_Skybox()
{

}

SGfx_Skybox::~SGfx_Skybox()
{

}

bool SGfx_Skybox::Init()
{
	mSkyCubeMap = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Black_1x1.dds");

	SC_Array<SC_Vector> vertices;
	SC_Array<uint32> indices;
	SGfx_Shapes::GenerateSphere(vertices, indices, 2, 200);

	SR_BufferResourceProperties vertexBufferResourceProps;
	vertexBufferResourceProps.mElementCount = vertices.Count();
	vertexBufferResourceProps.mElementSize = vertices.ElementStride();
	vertexBufferResourceProps.mBindFlags = SR_BufferBindFlag_VertexBuffer;
	mCubeVertices = SR_RenderDevice::gInstance->CreateBufferResource(vertexBufferResourceProps, vertices.GetBuffer());

	SR_BufferResourceProperties indexBufferResourceProps;
	indexBufferResourceProps.mElementCount = indices.Count();
	indexBufferResourceProps.mElementSize = indices.ElementStride();
	indexBufferResourceProps.mBindFlags = SR_BufferBindFlag_IndexBuffer;
	mCubeIndices = SR_RenderDevice::gInstance->CreateBufferResource(indexBufferResourceProps, indices.GetBuffer());

	SR_ShaderStateProperties cubeShaderProps;
	SR_ShaderCompileArgs compileArgs;
	compileArgs.mEntryPoint = "MainVS";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/SkySphere.ssf";
	compileArgs.mType = SR_ShaderType::Vertex;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, cubeShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Vertex)], &cubeShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Vertex)]))
		return false;
	compileArgs.mEntryPoint = "MainPS";
	compileArgs.mType = SR_ShaderType::Pixel;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, cubeShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Pixel)], &cubeShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Pixel)]))
		return false;

	cubeShaderProps.mVertexLayout.SetAttribute(SR_VertexAttribute::Position, SR_Format::RGB32_FLOAT);
	cubeShaderProps.mRTVFormats.mNumColorFormats = 1;
	cubeShaderProps.mRTVFormats.mColorFormats[0] = SR_Format::RG11B10_FLOAT;
	cubeShaderProps.mPrimitiveTopology = SR_PrimitiveTopology::TriangleList;

	cubeShaderProps.mDepthStencilProperties.mDepthComparisonFunc = SR_ComparisonFunc::GreaterEqual;

	mCubeShader = SR_RenderDevice::gInstance->CreateShaderState(cubeShaderProps);

	SR_BufferResourceProperties cbDesc;
	cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
	cbDesc.mElementCount = 1;
	cbDesc.mElementSize = sizeof(uint32);
	mDrawInfoBuffer = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);

	uint32 cubemapDescriptorIndex = mSkyCubeMap->GetDescriptorHeapIndex();
	mDrawInfoBuffer->UpdateData(0, &cubemapDescriptorIndex, sizeof(uint32));

	return true;
}

void SGfx_Skybox::Render(SR_CommandList* aCmdList)
{
	aCmdList->SetVertexBuffer(mCubeVertices.get());
	aCmdList->SetIndexBuffer(mCubeIndices.get());
	aCmdList->SetShaderState(mCubeShader.get());
	aCmdList->SetPrimitiveTopology(SR_PrimitiveTopology::TriangleList);
	aCmdList->SetRootConstantBuffer(mDrawInfoBuffer.get(), 0);
	aCmdList->DrawIndexed(mCubeIndices->GetProperties().mElementCount);
}

SGfx_Environment::SGfx_Environment()
{
	mConstants.mSunLightDirection = SC_Vector(0.45f, 0.80f, 0.05f);
	mConstants.mSunLightDirection.Normalize();

	mConstants.mSunLightIntensity = 140.0f;

	mConstants.mSunLightColor = SC_Vector(0.2f, 0.8f, 0.2f);

	mEnvironmentalIrradianceMap = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Black_1x1.dds");
	mPreFilteredEnvironmentMap = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Black_1x1.dds");
	mEnvironmentalBrdfLUT = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Black_1x1.dds");

	mConstants.mEnvironmentalIrradianceMapDescriptorIndex = mEnvironmentalIrradianceMap->GetDescriptorHeapIndex();
	mConstants.mPreFilteredEnvironmentMapDescriptorIndex = mPreFilteredEnvironmentMap->GetDescriptorHeapIndex();
	mConstants.mEnvironmentBrdfLUTDescriptorIndex = mEnvironmentalBrdfLUT->GetDescriptorHeapIndex();
	mConstants.mPreFilteredEnvironmentMapMips = mPreFilteredEnvironmentMap->GetResource()->GetProperties().mNumMips;


	mSkybox = SC_MakeUnique<SGfx_Skybox>();
	mSkybox->Init();
}

SGfx_Environment::~SGfx_Environment()
{

}

const SGfx_EnvironmentConstants& SGfx_Environment::GetConstants() const
{
	return mConstants;
}
