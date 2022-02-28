#include "SGfx_Environment.h"
#include "Graphics/View/SGfx_Camera.h"
#include "Graphics/Misc/SGfx_Shapes.h"
#include "Graphics/Misc/SGfx_DefaultTextures.h"
#include "RenderCore/Interface/SR_CommandList.h"

SGfx_Skybox::SGfx_Skybox()
{

}

SGfx_Skybox::~SGfx_Skybox()
{

}

bool SGfx_Skybox::Init()
{
	mSkyCubeMap = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Grey_Cube.dds");

	SC_Array<SC_Vector> vertices;
	SC_Array<uint32> indices;
	SGfx_Shapes::GenerateSphere(vertices, indices, 2, 100);

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
	mConstants.mSunLightIntensity = 30.0f;

	mConstants.mSunLightColor = SC_Vector(0.99f, 0.84f, 0.55f);

	mEnvironmentalIrradianceMap = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Grey_Cube.dds");
	mPreFilteredEnvironmentMap = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Grey_Cube.dds");
	mEnvironmentalBrdfLUT = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/EnvironmentBrdfLUT.dds");

	mConstants.mEnvironmentalIrradianceMapDescriptorIndex = mEnvironmentalIrradianceMap->GetDescriptorHeapIndex();
	mConstants.mPreFilteredEnvironmentMapDescriptorIndex = mPreFilteredEnvironmentMap->GetDescriptorHeapIndex();
	mConstants.mEnvironmentBrdfLUTDescriptorIndex = mEnvironmentalBrdfLUT->GetDescriptorHeapIndex();
	mConstants.mPreFilteredEnvironmentMapMips = mPreFilteredEnvironmentMap->GetResource()->GetProperties().mNumMips;

	// Scattering constants
	SGfx_ScatteringConstants& scatteringConstants = mConstants.mScatteringConstants;
	scatteringConstants.mPlanetRadiusKm = 1000.0f;
	constexpr float atmosphereHeightKm = 100.0f;
	scatteringConstants.mPlanetAtmosphereRadiusKm = scatteringConstants.mPlanetRadiusKm + SC_Max(0.1f, atmosphereHeightKm);

	scatteringConstants.mRayleighScattering = SC_Vector(0.005802f, 0.013558f, 0.033100f);
	scatteringConstants.mRayleighDensityExpScale = -1.0f / 8.0f;

	scatteringConstants.mMiePhaseG = 0.8f;
	scatteringConstants.mMieDensityExpScale = -1.0f / 1.2f;
	scatteringConstants.mMieScattering = SC_Vector(0.003996f, 0.003996f, 0.003996f);
	scatteringConstants.mMieExtinction = SC_Vector(0.004440f, 0.004440f, 0.004440f);
	scatteringConstants.mMieAbsorption.x = scatteringConstants.mMieExtinction.x - scatteringConstants.mMieScattering.x;
	scatteringConstants.mMieAbsorption.y = scatteringConstants.mMieExtinction.y - scatteringConstants.mMieScattering.y;
	scatteringConstants.mMieAbsorption.z = scatteringConstants.mMieExtinction.z - scatteringConstants.mMieScattering.z;

	scatteringConstants.mAbsorptionExtinction = SC_Vector(0.000650f, 0.001881f, 0.000085f);

	scatteringConstants.mAbsorptionDensity0LayerWidth = 25.0f;
	scatteringConstants.mAbsorptionDensity0ConstantTerm = -2.0f / 3.0f;
	scatteringConstants.mAbsorptionDensity0LinearTerm = 1.0f / 15.0f;
	scatteringConstants.mAbsorptionDensity1ConstantTerm = 8.0f / 3.0f;
	scatteringConstants.mAbsorptionDensity1LinearTerm = -1.0f / 15.0f;

	mTransmittanceLUT.Init({ 256, 64, 1 }, SR_Format::RG11B10_FLOAT, false, true, "Transmittance LUT");
	mSkyViewLUT.Init({ 192, 104, 1 }, SR_Format::RG11B10_FLOAT, false, true, "SkyView LUT");

	scatteringConstants.mTransmittanceLUTDescriptorIndex = mTransmittanceLUT.mTexture->GetDescriptorHeapIndex();

	SR_ShaderCompileArgs compileArgs;
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/ComputeScatteringLUT.ssf";
	compileArgs.mType = SR_ShaderType::Compute;
	{
		compileArgs.mEntryPoint = "ComputeTransmittanceLUT";
		SR_ShaderStateProperties computeLUTShaderProps;
		SR_RenderDevice::gInstance->CompileShader(compileArgs, computeLUTShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &computeLUTShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]);
		mComputeTransmittanceLUTShader = SR_RenderDevice::gInstance->CreateShaderState(computeLUTShaderProps);
	}

	{
		compileArgs.mEntryPoint = "ComputeSkyViewLUT";	
		SR_ShaderStateProperties computeLUTShaderProps;
		SR_RenderDevice::gInstance->CompileShader(compileArgs, computeLUTShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &computeLUTShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]);
		mComputeSkyViewLUTShader = SR_RenderDevice::gInstance->CreateShaderState(computeLUTShaderProps);
	}
	mSkybox = SC_MakeUnique<SGfx_Skybox>();
	if (!mSkybox->Init())
		mSkybox = nullptr;
		
}

SGfx_Environment::~SGfx_Environment()
{

}

const SGfx_EnvironmentConstants& SGfx_Environment::GetConstants() const
{
	return mConstants;
}

SC_Vector& SGfx_Environment::GetSunDirection()
{
	return mConstants.mSunLightDirection;
}

const SC_Vector& SGfx_Environment::GetSunDirection() const
{
	return mConstants.mSunLightDirection;
}

SC_Vector& SGfx_Environment::GetSunColor()
{
	return mConstants.mSunLightColor;
}

const SC_Vector& SGfx_Environment::GetSunColor() const
{
	return mConstants.mSunLightColor;
}

float& SGfx_Environment::GetSunIntensity()
{
	return mConstants.mSunLightIntensity;
}

const float& SGfx_Environment::GetSunIntensity() const
{
	return mConstants.mSunLightIntensity;
}

void SGfx_Environment::UpdateConstants(const SGfx_Camera& aWorldCamera)
{
	const SC_Vector cameraPos = aWorldCamera.GetPosition();

	constexpr float M_TO_SKY_UNIT = 0.001f; // Engine units are in meters
	constexpr float SKY_UNIT_TO_M = 1.0f / M_TO_SKY_UNIT;
	constexpr float planetRadiusOffset = 0.005f; // Always force to be 5 meters above the ground/sea level

	const SC_Vector planetCenterKm(0.0f, -mConstants.mScatteringConstants.mPlanetRadiusKm, 0.0f);

	const float offset = planetRadiusOffset * SKY_UNIT_TO_M;
	const float planetRadiusWorld = mConstants.mScatteringConstants.mPlanetRadiusKm * SKY_UNIT_TO_M;
	const SC_Vector planetCenterWorld = planetCenterKm * SKY_UNIT_TO_M;
	const SC_Vector planetCenterToCameraWorld = cameraPos - planetCenterWorld;
	const float distanceToPlanetCenterWorld = planetCenterToCameraWorld.Length();

	if (distanceToPlanetCenterWorld < (planetRadiusWorld + offset))
		mConstants.mScatteringConstants.mVirtualSkyCameraPosition = planetCenterWorld + SC_Vector(planetRadiusWorld + offset) * (planetCenterToCameraWorld / distanceToPlanetCenterWorld);
	else
		mConstants.mScatteringConstants.mVirtualSkyCameraPosition = cameraPos;

	mConstants.mScatteringConstants.mPlanetCenterAndViewHeight = SC_Vector4(planetCenterWorld, (mConstants.mScatteringConstants.mVirtualSkyCameraPosition - planetCenterWorld).Length());

	mConstants.mSunLightDirection.Normalize();
}

void SGfx_Environment::ComputeScatteringLUTs(SR_CommandList* aCmdList)
{
	struct Constants
	{
		SC_Vector4 mTargetSizeAndInvSize;
		uint32 mTransmittanceLUTDescriptorIndex;
		uint32 mMultiScatteringLUTDescriptorIndex;
		uint32 mOutputTextureDescriptorIndex;
	};

	{
		aCmdList->BeginEvent("Transmittance LUT");

		SC_IntVector targetSize = mTransmittanceLUT.GetResourceProperties().mSize;

		Constants constants;
		constants.mTargetSizeAndInvSize = SC_Vector4((float)targetSize.x, (float)targetSize.y, 1.0f / targetSize.x, 1.0f / targetSize.y);
		constants.mTransmittanceLUTDescriptorIndex = SGfx_DefaultTextures::GetWhite1x1()->GetDescriptorHeapIndex();
		constants.mMultiScatteringLUTDescriptorIndex = SGfx_DefaultTextures::GetBlack1x1()->GetDescriptorHeapIndex();
		constants.mOutputTextureDescriptorIndex = mTransmittanceLUT.mTextureRW->GetDescriptorHeapIndex();

		if (!mTransmittanceLUTConstantBuffer)
		{
			SR_BufferResourceProperties cbDesc;
			cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
			cbDesc.mElementCount = 1;
			cbDesc.mElementSize = sizeof(Constants);
			mTransmittanceLUTConstantBuffer = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
		}

		mTransmittanceLUTConstantBuffer->UpdateData(0, &constants, sizeof(constants));

		aCmdList->SetRootConstantBuffer(mTransmittanceLUTConstantBuffer.get(), 0);
		aCmdList->TransitionBarrier(SR_ResourceState_UnorderedAccess, mTransmittanceLUT.mResource.get());
		aCmdList->Dispatch(mComputeTransmittanceLUTShader.get(), targetSize.x, targetSize.y);
		aCmdList->TransitionBarrier(SR_ResourceState_Read, mTransmittanceLUT.mResource.get());

		aCmdList->EndEvent();
	}
	{

		aCmdList->BeginEvent("SkyView LUT");

		SC_IntVector targetSize = mSkyViewLUT.GetResourceProperties().mSize;

		Constants constants;
		constants.mTargetSizeAndInvSize = SC_Vector4((float)targetSize.x, (float)targetSize.y, 1.0f / targetSize.x, 1.0f / targetSize.y);
		constants.mTransmittanceLUTDescriptorIndex = SGfx_DefaultTextures::GetBlack1x1()->GetDescriptorHeapIndex();
		constants.mMultiScatteringLUTDescriptorIndex = SGfx_DefaultTextures::GetBlack1x1()->GetDescriptorHeapIndex();
		constants.mOutputTextureDescriptorIndex = mSkyViewLUT.mTextureRW->GetDescriptorHeapIndex();

		if (!mSkyViewLUTConstantBuffer)
		{
			SR_BufferResourceProperties cbDesc;
			cbDesc.mBindFlags = SR_BufferBindFlag_ConstantBuffer;
			cbDesc.mElementCount = 1;
			cbDesc.mElementSize = sizeof(Constants);
			mSkyViewLUTConstantBuffer = SR_RenderDevice::gInstance->CreateBufferResource(cbDesc);
		}

		mSkyViewLUTConstantBuffer->UpdateData(0, &constants, sizeof(constants));

		aCmdList->SetRootConstantBuffer(mSkyViewLUTConstantBuffer.get(), 0);
		aCmdList->TransitionBarrier(SR_ResourceState_UnorderedAccess, mSkyViewLUT.mResource.get());
		aCmdList->Dispatch(mComputeSkyViewLUTShader.get(), targetSize.x, targetSize.y);
		aCmdList->TransitionBarrier(SR_ResourceState_Read, mSkyViewLUT.mResource.get());

		aCmdList->EndEvent();
	}
}

SR_Texture* SGfx_Environment::GetTransmittanceLUT() const
{
	return mTransmittanceLUT.mTexture.get();
}

SR_Texture* SGfx_Environment::GetSkyViewLUT() const
{
	return mSkyViewLUT.mTexture.get();
}
