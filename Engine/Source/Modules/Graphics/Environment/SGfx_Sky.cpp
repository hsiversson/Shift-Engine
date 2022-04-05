#include "SGfx_Sky.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Misc/SGfx_Shapes.h"
#include "Graphics/Misc/SGfx_DefaultTextures.h"

SGfx_Sky::SGfx_Sky()
{

}

SGfx_Sky::~SGfx_Sky()
{

}

bool SGfx_Sky::Init()
{
	if (!InitSkySphere())
		return false;

	if (!InitSkyAtmosphere())
		return false;

	if (!InitSkyIrradianceMaps())
		return false;

	return true;
}

void SGfx_Sky::UpdateConstants(const SGfx_Camera& aWorldCamera)
{
	const SC_Vector cameraPos = aWorldCamera.GetPosition();

	constexpr float M_TO_SKY_UNIT = 0.001f; // Engine units are in meters
	constexpr float SKY_UNIT_TO_M = 1.0f / M_TO_SKY_UNIT;
	constexpr float planetRadiusOffset = 0.005f; // Always force to be 5 meters above the ground/sea level

	const SC_Vector planetCenterKm(0.0f, -mSkyAtmosphereConstants.mPlanetRadiusKm, 0.0f);

	const float offset = planetRadiusOffset * SKY_UNIT_TO_M;
	const float planetRadiusWorld = mSkyAtmosphereConstants.mPlanetRadiusKm * SKY_UNIT_TO_M;
	const SC_Vector planetCenterWorld = planetCenterKm * SKY_UNIT_TO_M;
	const SC_Vector planetCenterToCameraWorld = cameraPos - planetCenterWorld;
	const float distanceToPlanetCenterWorld = planetCenterToCameraWorld.Length();

	if (distanceToPlanetCenterWorld < (planetRadiusWorld + offset))
		mSkyAtmosphereConstants.mVirtualSkyCameraPosition = planetCenterWorld + SC_Vector(planetRadiusWorld + offset) * (planetCenterToCameraWorld / distanceToPlanetCenterWorld);
	else
		mSkyAtmosphereConstants.mVirtualSkyCameraPosition = cameraPos;

	mSkyAtmosphereConstants.mPlanetCenterAndViewHeight = SC_Vector4(planetCenterWorld, (mSkyAtmosphereConstants.mVirtualSkyCameraPosition - planetCenterWorld).Length());
}

const SGfx_SkyAtmosphereConstants& SGfx_Sky::GetSkyAtmosphereConstants() const
{
	return mSkyAtmosphereConstants;
}

const SGfx_SkyIrradianceConstants& SGfx_Sky::GetSkyIrradianceConstants() const
{
	return mSkyIrradianceConstants;
}

void SGfx_Sky::ComputeSkyIrradiance(SR_CommandList* /*aCmdList*/)
{
}

void SGfx_Sky::ComputeSkyAtmosphereLUTs(SR_CommandList* aCmdList)
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

bool SGfx_Sky::InitSkySphere()
{
	SC_Array<SC_Vector> vertices;
	SC_Array<uint32> indices;
	SGfx_Shapes::GenerateSphere(vertices, indices, 2, 100);

	SR_BufferResourceProperties vertexBufferResourceProps;
	vertexBufferResourceProps.mElementCount = vertices.Count();
	vertexBufferResourceProps.mElementSize = vertices.ElementStride();
	vertexBufferResourceProps.mBindFlags = SR_BufferBindFlag_VertexBuffer;
	mSphereVertexBuffer = SR_RenderDevice::gInstance->CreateBufferResource(vertexBufferResourceProps, vertices.GetBuffer());

	SR_BufferResourceProperties indexBufferResourceProps;
	indexBufferResourceProps.mElementCount = indices.Count();
	indexBufferResourceProps.mElementSize = indices.ElementStride();
	indexBufferResourceProps.mBindFlags = SR_BufferBindFlag_IndexBuffer;
	mSphereIndexBuffer = SR_RenderDevice::gInstance->CreateBufferResource(indexBufferResourceProps, indices.GetBuffer());

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

	mSphereShader = SR_RenderDevice::gInstance->CreateShaderState(cubeShaderProps);
	return true;
}

bool SGfx_Sky::InitSkyAtmosphere()
{
	constexpr float atmosphereHeightKm = 100.0f;

	mSkyAtmosphereConstants.mPlanetRadiusKm = 1000.0f;
	mSkyAtmosphereConstants.mPlanetAtmosphereRadiusKm = mSkyAtmosphereConstants.mPlanetRadiusKm + SC_Max(0.1f, atmosphereHeightKm);

	mSkyAtmosphereConstants.mRayleighScattering = SC_Vector(0.005802f, 0.013558f, 0.033100f);
	mSkyAtmosphereConstants.mRayleighDensityExpScale = -1.0f / 8.0f;

	mSkyAtmosphereConstants.mMiePhaseG = 0.8f;
	mSkyAtmosphereConstants.mMieDensityExpScale = -1.0f / 1.2f;
	mSkyAtmosphereConstants.mMieScattering = SC_Vector(0.003996f, 0.003996f, 0.003996f);
	mSkyAtmosphereConstants.mMieExtinction = SC_Vector(0.004440f, 0.004440f, 0.004440f);
	mSkyAtmosphereConstants.mMieAbsorption.x = mSkyAtmosphereConstants.mMieExtinction.x - mSkyAtmosphereConstants.mMieScattering.x;
	mSkyAtmosphereConstants.mMieAbsorption.y = mSkyAtmosphereConstants.mMieExtinction.y - mSkyAtmosphereConstants.mMieScattering.y;
	mSkyAtmosphereConstants.mMieAbsorption.z = mSkyAtmosphereConstants.mMieExtinction.z - mSkyAtmosphereConstants.mMieScattering.z;

	mSkyAtmosphereConstants.mAbsorptionExtinction = SC_Vector(0.000650f, 0.001881f, 0.000085f);

	mSkyAtmosphereConstants.mAbsorptionDensity0LayerWidth = 25.0f;
	mSkyAtmosphereConstants.mAbsorptionDensity0ConstantTerm = -2.0f / 3.0f;
	mSkyAtmosphereConstants.mAbsorptionDensity0LinearTerm = 1.0f / 15.0f;
	mSkyAtmosphereConstants.mAbsorptionDensity1ConstantTerm = 8.0f / 3.0f;
	mSkyAtmosphereConstants.mAbsorptionDensity1LinearTerm = -1.0f / 15.0f;

	if (!mTransmittanceLUT.Init({ 256, 64, 1 }, SR_Format::RG11B10_FLOAT, false, true, "Transmittance LUT"))
		return false;
	if (!mSkyViewLUT.Init({ 192, 104, 1 }, SR_Format::RG11B10_FLOAT, false, true, "SkyView LUT"))
		return false;

	mSkyAtmosphereConstants.mTransmittanceLUTDescriptorIndex = mTransmittanceLUT.mTexture->GetDescriptorHeapIndex();

	SR_ShaderCompileArgs compileArgs;
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/ComputeScatteringLUT.ssf";
	compileArgs.mType = SR_ShaderType::Compute;
	{
		compileArgs.mEntryPoint = "ComputeTransmittanceLUT";
		SR_ShaderStateProperties computeLUTShaderProps;
		if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, computeLUTShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &computeLUTShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
			return false;
		mComputeTransmittanceLUTShader = SR_RenderDevice::gInstance->CreateShaderState(computeLUTShaderProps);
	}

	{
		compileArgs.mEntryPoint = "ComputeSkyViewLUT";
		SR_ShaderStateProperties computeLUTShaderProps;
		if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, computeLUTShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &computeLUTShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
			return false;
		mComputeSkyViewLUTShader = SR_RenderDevice::gInstance->CreateShaderState(computeLUTShaderProps);
	}

	return true;
}

bool SGfx_Sky::InitSkyIrradianceMaps()
{
	mDiffuseIrradianceMap = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Black_1x1.dds");
	mPreFilteredSpecularIrradiaceMap = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Black_1x1.dds");
	mIrradianceBrdfLUT = SR_RenderDevice::gInstance->LoadTexture(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/EnvironmentBrdfLUT.dds");

	mSkyIrradianceConstants.mDiffuseIrradianceMapDescriptorIndex = mDiffuseIrradianceMap->GetDescriptorHeapIndex();
	mSkyIrradianceConstants.mPreFilteredSpecularIrradianceMapDescriptorIndex = mPreFilteredSpecularIrradiaceMap->GetDescriptorHeapIndex();
	mSkyIrradianceConstants.mIrradianceBrdfLUTDescriptorIndex = mIrradianceBrdfLUT->GetDescriptorHeapIndex();
	mSkyIrradianceConstants.mNumPreFilteredIrradianceMapMips = mPreFilteredSpecularIrradiaceMap->GetResource()->GetProperties().mNumMips;
	return true;
}

void SGfx_Sky::Render(SR_CommandList* aCmdList)
{
	aCmdList->SetVertexBuffer(mSphereVertexBuffer.get());
	aCmdList->SetIndexBuffer(mSphereIndexBuffer.get());
	aCmdList->SetShaderState(mSphereShader.get());
	aCmdList->SetPrimitiveTopology(SR_PrimitiveTopology::TriangleList);
	aCmdList->DrawIndexed(mSphereIndexBuffer->GetProperties().mElementCount);
}
