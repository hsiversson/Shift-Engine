#include "SGfx_Raytracing.h"
#include "Graphics/View/SGfx_ViewData.h"
#include "Graphics/View/SGfx_View.h"
#include "Graphics/Misc/SGfx_Shapes.h"

bool SGfx_DDGI::Init()
{
	const SC_Vector minExtents = SC_Vector(-16, -3, -16);
	const SC_Vector maxExtents = SC_Vector(16, 3, 16);

	SC_Vector sceneLength = maxExtents - minExtents;

	mProbeGridProperties.mDistanceBetweenProbes = 1.5f;
	mProbeGridProperties.mNumProbes = SC_IntVector(sceneLength / mProbeGridProperties.mDistanceBetweenProbes) + SC_IntVector(2);
	mProbeGridProperties.mStartPosition = minExtents;
	mProbeGridProperties.mMaxDistance = 4.0f;
	mProbeGridProperties.mIrradianceProbeSideLength = 8;
	mProbeGridProperties.mDepthProbeSideLength = 16;
	mProbeGridProperties.mRaysPerProbe = 256;
	mProbeGridProperties.mDepthSharpness = 50.0f;
	mProbeGridProperties.mHistoryFactor = 0.98f;
	mProbeGridProperties.mNormalBias = 0.25f;
	mProbeGridProperties.mEnergyConservation = 0.85f;
	mProbeGridProperties.mGlobalRoughnessMultiplier = 1.00f;
	mProbeGridProperties.mDiffuseEnabled = true;
	mProbeGridProperties.mSpecularEnabled = true;
	mProbeGridProperties.mVisualizeProbes = false;
	mProbeGridProperties.mInfiniteBounces = true;

	if (!CreateShaders())
		return false;

	UpdateResources();

	if (!InitDebugResources())
		return false;

	return true;
}

void SGfx_DDGI::GetConstants(SGfx_ViewData& aPrepareData)
{
	Constants& giConstants = aPrepareData.mSceneConstants.mGIConstants;

	SC_Vector camPos = aPrepareData.mSceneConstants.mViewConstants.mCameraPosition;
	camPos.x = roundf(camPos.x);
	camPos.y = roundf(camPos.y);
	camPos.z = roundf(camPos.z);

	giConstants.mGridStartPosition = mProbeGridProperties.mStartPosition + camPos;
	giConstants.mGridStep = SC_Vector(mProbeGridProperties.mDistanceBetweenProbes);
	giConstants.mNumProbes = mProbeGridProperties.mNumProbes;
	giConstants.mMaxDistance = mProbeGridProperties.mMaxDistance;
	giConstants.mDepthSharpness = mProbeGridProperties.mDepthSharpness;
	giConstants.mHistoryFactor = mProbeGridProperties.mHistoryFactor;
	giConstants.mNormalBias = mProbeGridProperties.mNormalBias;
	giConstants.mEnergyConservation = mProbeGridProperties.mEnergyConservation;
	giConstants.mIrradianceProbeSideLength = mProbeGridProperties.mIrradianceProbeSideLength;
	giConstants.mIrradianceTextureDimensions = mProbeIrradianceTexture->GetResourceProperties().mSize.XY();
	giConstants.mIrradianceTextureDescriptorIndex = mProbeIrradianceTexture->GetDescriptorHeapIndex();
	giConstants.mDepthProbeSideLength = mProbeGridProperties.mDepthProbeSideLength;
	giConstants.mDepthTextureDimensions = mProbeDepthTexture->GetResourceProperties().mSize.XY();
	giConstants.mDepthTextureDescriptorIndex = mProbeDepthTexture->GetDescriptorHeapIndex();;
	giConstants.mRaysPerProbe = mProbeGridProperties.mRaysPerProbe;
	giConstants.mEnableVisibilityTesting = true;
	giConstants.mDiffuseGIDescriptorIndex = mDiffuseGITexture->GetDescriptorHeapIndex();
	giConstants.mReflectionsDescriptorIndex = mReflectionsTexture->GetDescriptorHeapIndex();
	giConstants.mGlobalRoughnessMultiplier = mProbeGridProperties.mGlobalRoughnessMultiplier;
	giConstants.mDiffuseEnabled = mProbeGridProperties.mDiffuseEnabled;
	giConstants.mSpecularEnabled = mProbeGridProperties.mSpecularEnabled;
	giConstants.mInfiniteBounces = (aPrepareData.mSceneConstants.mFrameIndex == 0) ? 0 : mProbeGridProperties.mInfiniteBounces;
}

void SGfx_DDGI::RenderDiffuse(SGfx_View* aView)
{
	if (!mProbeGridProperties.mDiffuseEnabled)
		return;

	const SGfx_ViewData& renderData = aView->GetRenderData();
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	cmdList->BeginEvent("SGfx_DDGI::RenderDiffuse");

	SC_Array<SC_Pair<uint32, SR_TrackedResource*>> transitions;
	transitions.Add(SC_Pair(SR_ResourceState_UnorderedAccess, mTraceRaysRadianceTextureRW->GetResource()));
	transitions.Add(SC_Pair(SR_ResourceState_UnorderedAccess, mTraceRaysDirectionDepthTextureRW->GetResource()));
	transitions.Add(SC_Pair(SR_ResourceState_UnorderedAccess, mProbeIrradianceTextureRW->GetResource()));
	transitions.Add(SC_Pair(SR_ResourceState_UnorderedAccess, mProbeDepthTextureRW->GetResource()));
	cmdList->TransitionBarrier(transitions);

	// TraceRays
	{
		SC_PROFILER_EVENT_SCOPED("SGfx_DDGI::TraceRays");
		cmdList->BeginEvent("SGfx_DDGI::TraceRays");

		struct TraceConstants
		{
			SC_Matrix mRandomOrientation;
			uint32 mIrradianceTextureDescriptorIndex;
			uint32 mDirectionDistDescriptorIndex;
		} constants;

		float angle = SC_Random::Range(0.0f, 2.0f) * SC_Math::PI;
		SC_Vector axis = SC_Vector(SC_Random::Range(-1.0f, 1.0f), SC_Random::Range(-1.0f, 1.0f), SC_Random::Range(-1.0f, 1.0f));
		axis.Normalize();
		constants.mRandomOrientation = SC_Matrix::CreateRotation(axis, angle);

		constants.mIrradianceTextureDescriptorIndex = mTraceRaysRadianceTextureRW->GetDescriptorHeapIndex();
		constants.mDirectionDistDescriptorIndex = mTraceRaysDirectionDepthTextureRW->GetDescriptorHeapIndex();
		cmdList->SetRootConstantBuffer(sizeof(constants), &constants, 0);

		uint32 totalProbeCount = mProbeGridProperties.mNumProbes.x * mProbeGridProperties.mNumProbes.y * mProbeGridProperties.mNumProbes.z;
		cmdList->Dispatch(mTraceRaysShader.Get(), mProbeGridProperties.mRaysPerProbe, totalProbeCount, 1);

		transitions.RemoveAll();
		transitions.Add(SC_Pair(SR_ResourceState_Read, mTraceRaysRadianceTextureRW->GetResource()));
		transitions.Add(SC_Pair(SR_ResourceState_Read, mTraceRaysDirectionDepthTextureRW->GetResource()));
		cmdList->TransitionBarrier(transitions);

		cmdList->EndEvent();
	}

	// UpdateProbes
	{
		SC_PROFILER_EVENT_SCOPED("SGfx_DDGI::UpdateProbes");
		cmdList->BeginEvent("SGfx_DDGI::UpdateProbes");

		struct UpdateConstants
		{
			SC_Vector2 mTextureSize;
			float mProbeSideLength;
			uint32 mTextureDescriptorIndex;

			uint32 mDirectionDepthDescriptorIndex;
			uint32 mRadianceDescriptorIndex;
		} constants;

		constants.mDirectionDepthDescriptorIndex = mTraceRaysDirectionDepthTexture->GetDescriptorHeapIndex();
		constants.mRadianceDescriptorIndex = mTraceRaysRadianceTexture->GetDescriptorHeapIndex();

		constants.mTextureDescriptorIndex = mProbeIrradianceTextureRW->GetDescriptorHeapIndex();
		constants.mTextureSize = mProbeIrradianceTextureRW->GetResourceProperties().mSize.XY();
		constants.mProbeSideLength = (float)mProbeGridProperties.mIrradianceProbeSideLength;

		uint32 dispatchX = mProbeGridProperties.mNumProbes.x * mProbeGridProperties.mNumProbes.y;
		uint32 dispatchY = mProbeGridProperties.mNumProbes.z;
		cmdList->SetRootConstantBuffer(sizeof(constants), &constants, 0);
		cmdList->SetShaderState(mUpdateProbesShader[1].Get());
		cmdList->Dispatch(dispatchX, dispatchY, 1); // Irradiance Update

		constants.mTextureDescriptorIndex = mProbeDepthTextureRW->GetDescriptorHeapIndex();
		constants.mTextureSize = mProbeDepthTextureRW->GetResourceProperties().mSize.XY();
		constants.mProbeSideLength = (float)mProbeGridProperties.mDepthProbeSideLength;

		cmdList->SetRootConstantBuffer(sizeof(constants), &constants, 0);
		cmdList->SetShaderState(mUpdateProbesShader[0].Get());
		cmdList->Dispatch(dispatchX, dispatchY, 1); // Depth Update

		cmdList->EndEvent();
	}
	{
		SC_PROFILER_EVENT_SCOPED("SGfx_DDGI::UpdateProbeBorders");
		cmdList->BeginEvent("SGfx_DDGI::UpdateProbeBorders");

		uint32 descriptorIndex = mProbeIrradianceTextureRW->GetDescriptorHeapIndex();
		cmdList->SetRootConstantBuffer(sizeof(descriptorIndex), &descriptorIndex, 0);

		uint32 dispatchX = mProbeGridProperties.mNumProbes.x * mProbeGridProperties.mNumProbes.y;
		uint32 dispatchY = mProbeGridProperties.mNumProbes.z;
		cmdList->SetShaderState(mUpdateProbesBorderShader[1].Get());
		cmdList->Dispatch(dispatchX, dispatchY, 1); // Irradiance Border Update

		descriptorIndex = mProbeDepthTextureRW->GetDescriptorHeapIndex();
		cmdList->SetRootConstantBuffer(sizeof(descriptorIndex), &descriptorIndex, 0);
		cmdList->SetShaderState(mUpdateProbesBorderShader[0].Get());
		cmdList->Dispatch(dispatchX, dispatchY, 1); // Depth Border Update

		transitions.RemoveAll();
		transitions.Add(SC_Pair(SR_ResourceState_Read, mProbeIrradianceTextureRW->GetResource()));
		transitions.Add(SC_Pair(SR_ResourceState_Read, mProbeDepthTextureRW->GetResource()));
		transitions.Add(SC_Pair(SR_ResourceState_UnorderedAccess, mDiffuseGITextureRW->GetResource()));
		cmdList->TransitionBarrier(transitions);
		cmdList->EndEvent();
	}
	{
		SC_PROFILER_EVENT_SCOPED("SGfx_RTGI::SampleGrid");
		cmdList->BeginEvent("SGfx_RTGI::SampleGrid");

		struct Constants
		{
			SC_Vector4 mTargetResolutionAndRcp;
			uint32 mOutputTextureDescriptorIndex;
			uint32 _unused[3];
		} constants;

		constants.mTargetResolutionAndRcp = SC_Vector4(mDiffuseGITextureRW->GetResourceProperties().mSize.XY(), SC_Vector2(1.0f) / mDiffuseGITextureRW->GetResourceProperties().mSize.XY());
		constants.mOutputTextureDescriptorIndex = mDiffuseGITextureRW->GetDescriptorHeapIndex();
		cmdList->SetRootConstantBuffer(sizeof(constants), &constants, 0);

		uint32 dispatchX = (uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x;
		uint32 dispatchY = (uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y;
		cmdList->Dispatch(mSampleProbesShader, dispatchX, dispatchY, 1);

		cmdList->TransitionBarrier(SR_ResourceState_Read, mDiffuseGITexture->GetResource());
		cmdList->EndEvent();
	}

	cmdList->EndEvent();
}

void SGfx_DDGI::RenderSpecular(SGfx_View* aView)
{
	if (!mProbeGridProperties.mSpecularEnabled)
		return;

	const SGfx_ViewData& renderData = aView->GetRenderData();
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	cmdList->BeginEvent("SGfx_DDGI::RenderSpecular");
	cmdList->TransitionBarrier(SR_ResourceState_UnorderedAccess, mReflectionsTextureRW->GetResource());

	struct Constants
	{
		SC_Vector4 mTargetResolutionAndRcp;
		uint32 mOutputTextureDescriptorIndex;
		uint32 _unused[3];
	} constants;

	constants.mTargetResolutionAndRcp = SC_Vector4(mReflectionsTextureRW->GetResourceProperties().mSize.XY(), SC_Vector2(1.0f) / mReflectionsTextureRW->GetResourceProperties().mSize.XY());
	constants.mOutputTextureDescriptorIndex = mReflectionsTextureRW->GetDescriptorHeapIndex();
	cmdList->SetRootConstantBuffer(sizeof(constants), &constants, 0);

	uint32 dispatchX = (uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.x;
	uint32 dispatchY = (uint32)renderData.mSceneConstants.mViewConstants.mViewportSizeAndScale.y;
	cmdList->Dispatch(mReflectionsTraceRaysShader, dispatchX, dispatchY, 1);

	cmdList->TransitionBarrier(SR_ResourceState_Read, mReflectionsTextureRW->GetResource());
	cmdList->EndEvent();
}

void SGfx_DDGI::DebugRenderProbes(SGfx_View* /*aView*/)
{
	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->GetTaskCommandList();
	cmdList->BeginEvent("SGfx_DDGI::DebugRenderProbes");
	
	cmdList->SetVertexBuffer(mSphereVertexBuffer);
	cmdList->SetIndexBuffer(mSphereIndexBuffer);
	cmdList->SetShaderState(mDebugSphereShader);
	cmdList->SetPrimitiveTopology(SR_PrimitiveTopology::TriangleList);

	uint32 totalProbeCount = mProbeGridProperties.mNumProbes.x * mProbeGridProperties.mNumProbes.y * mProbeGridProperties.mNumProbes.z;
	cmdList->DrawIndexedInstanced(mSphereIndexBuffer->GetProperties().mElementCount, totalProbeCount);

	cmdList->EndEvent();
}

bool SGfx_DDGI::CreateShaders()
{
	SR_ShaderCompileArgs compileArgs;
	compileArgs.mEntryPoint = "Main";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/Raytracing/DDGI_TraceProbeRays.ssf";
	compileArgs.mType = SR_ShaderType::Compute;

	SR_ShaderStateProperties shaderProps;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, shaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &shaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mTraceRaysShader = SR_RenderDevice::gInstance->CreateShaderState(shaderProps);

	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/Raytracing/DDGI_ProbeUpdate.ssf";
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, shaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &shaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mUpdateProbesShader[1] = SR_RenderDevice::gInstance->CreateShaderState(shaderProps);

	compileArgs.mDefines.Add(SC_Pair<std::string, std::string>("PROBE_DEPTH_UPDATE", "1"));
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, shaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &shaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mUpdateProbesShader[0] = SR_RenderDevice::gInstance->CreateShaderState(shaderProps);

	compileArgs.mDefines.RemoveAll();
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/Raytracing/DDGI_ProbeBorderUpdate.ssf";
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, shaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &shaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mUpdateProbesBorderShader[1] = SR_RenderDevice::gInstance->CreateShaderState(shaderProps);

	compileArgs.mDefines.Add(SC_Pair<std::string, std::string>("PROBE_DEPTH_UPDATE", "1"));
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, shaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &shaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mUpdateProbesBorderShader[0] = SR_RenderDevice::gInstance->CreateShaderState(shaderProps);
	compileArgs.mDefines.RemoveAll();

	compileArgs.mEntryPoint = "Main";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/Raytracing/DDGI_SampleProbes.ssf";
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, shaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &shaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mSampleProbesShader = SR_RenderDevice::gInstance->CreateShaderState(shaderProps);

	compileArgs.mEntryPoint = "Main";
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/Raytracing/RTR_TraceRays.ssf"; 
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, shaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Compute)], &shaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Compute)]))
		return false;
	mReflectionsTraceRaysShader = SR_RenderDevice::gInstance->CreateShaderState(shaderProps);

	return true;
}

bool SGfx_DDGI::CreateTextures()
{
	uint32 totalProbeCount = mProbeGridProperties.mNumProbes.x * mProbeGridProperties.mNumProbes.y * mProbeGridProperties.mNumProbes.z;

	{
		SR_TextureResourceProperties radianceResourceProps;
		radianceResourceProps.mType = SR_ResourceType::Texture2D;
		radianceResourceProps.mFormat = SR_Format::RGBA16_FLOAT;
		radianceResourceProps.mSize = SC_IntVector(mProbeGridProperties.mRaysPerProbe, totalProbeCount, 1);
		radianceResourceProps.mAllowUnorderedAccess = true;
		radianceResourceProps.mDebugName = "DDGI Trace Rays Radiance";
		SC_Ref<SR_TextureResource> radianceResource = SR_RenderDevice::gInstance->CreateTextureResource(radianceResourceProps);

		SR_TextureProperties radianceTextureProps(radianceResourceProps.mFormat);
		mTraceRaysRadianceTexture = SR_RenderDevice::gInstance->CreateTexture(radianceTextureProps, radianceResource);
		radianceTextureProps.mWritable = true;
		mTraceRaysRadianceTextureRW = SR_RenderDevice::gInstance->CreateTexture(radianceTextureProps, radianceResource);
	} 
	{
		SR_TextureResourceProperties dirDepthResourceProps;
		dirDepthResourceProps.mType = SR_ResourceType::Texture2D;
		dirDepthResourceProps.mFormat = SR_Format::RGBA16_FLOAT;
		dirDepthResourceProps.mSize = SC_IntVector(mProbeGridProperties.mRaysPerProbe, totalProbeCount, 1);
		dirDepthResourceProps.mAllowUnorderedAccess = true;
		dirDepthResourceProps.mDebugName = "DDGI TraceRays Direction Depth";
		SC_Ref<SR_TextureResource> traceRaysDirectionDepthResource = SR_RenderDevice::gInstance->CreateTextureResource(dirDepthResourceProps);

		SR_TextureProperties dirDepthTextureProps(dirDepthResourceProps.mFormat);
		mTraceRaysDirectionDepthTexture = SR_RenderDevice::gInstance->CreateTexture(dirDepthTextureProps, traceRaysDirectionDepthResource);
		dirDepthTextureProps.mWritable = true;
		mTraceRaysDirectionDepthTextureRW = SR_RenderDevice::gInstance->CreateTexture(dirDepthTextureProps, traceRaysDirectionDepthResource);
	}

	{
		uint32 sideLength = mProbeGridProperties.mIrradianceProbeSideLength + 2;
		SC_IntVector2 resolution(sideLength * mProbeGridProperties.mNumProbes.x * mProbeGridProperties.mNumProbes.y + 2, sideLength * mProbeGridProperties.mNumProbes.z + 2);
		SR_TextureResourceProperties irradianceResourceProps;
		irradianceResourceProps.mType = SR_ResourceType::Texture2D;
		irradianceResourceProps.mFormat = SR_Format::RG11B10_FLOAT;
		irradianceResourceProps.mSize = SC_IntVector(resolution, 1);
		irradianceResourceProps.mAllowUnorderedAccess = true;
		irradianceResourceProps.mDebugName = "DDGI Irradiance Probe Grid";
		SC_Ref<SR_TextureResource> irradianceResource = SR_RenderDevice::gInstance->CreateTextureResource(irradianceResourceProps);

		SR_TextureProperties irradianceTextureProps(irradianceResourceProps.mFormat);
		mProbeIrradianceTexture = SR_RenderDevice::gInstance->CreateTexture(irradianceTextureProps, irradianceResource); 
		irradianceTextureProps.mWritable = true;
		mProbeIrradianceTextureRW = SR_RenderDevice::gInstance->CreateTexture(irradianceTextureProps, irradianceResource);
	}
	{
		uint32 sideLength = mProbeGridProperties.mDepthProbeSideLength + 2;
		SC_IntVector2 resolution(sideLength * mProbeGridProperties.mNumProbes.x * mProbeGridProperties.mNumProbes.y + 2, sideLength * mProbeGridProperties.mNumProbes.z + 2);
		SR_TextureResourceProperties depthResourceProps;
		depthResourceProps.mType = SR_ResourceType::Texture2D;
		depthResourceProps.mFormat = SR_Format::RG16_FLOAT;
		depthResourceProps.mSize = SC_IntVector(resolution, 1);
		depthResourceProps.mAllowUnorderedAccess = true;
		depthResourceProps.mDebugName = "DDGI Depth Probe Grid";
		SC_Ref<SR_TextureResource> depthResource = SR_RenderDevice::gInstance->CreateTextureResource(depthResourceProps);

		SR_TextureProperties depthTextureProps(depthResourceProps.mFormat);
		mProbeDepthTexture = SR_RenderDevice::gInstance->CreateTexture(depthTextureProps, depthResource);
		depthTextureProps.mWritable = true;
		mProbeDepthTextureRW = SR_RenderDevice::gInstance->CreateTexture(depthTextureProps, depthResource);
	}
	{
		SR_TextureResourceProperties resourceProps;
		resourceProps.mType = SR_ResourceType::Texture2D;
		resourceProps.mFormat = SR_Format::RG11B10_FLOAT;
		resourceProps.mSize = SC_IntVector(SR_RenderDevice::gInstance->GetSwapChain()->GetProperties().mSize, 1);
		resourceProps.mAllowUnorderedAccess = true;
		resourceProps.mDebugName = "DiffuseGI";
		SC_Ref<SR_TextureResource> ddgiResource = SR_RenderDevice::gInstance->CreateTextureResource(resourceProps);

		SR_TextureProperties textureProps(resourceProps.mFormat);
		mDiffuseGITexture = SR_RenderDevice::gInstance->CreateTexture(textureProps, ddgiResource);
		textureProps.mWritable = true;
		mDiffuseGITextureRW = SR_RenderDevice::gInstance->CreateTexture(textureProps, ddgiResource);
	}
	{
		SR_TextureResourceProperties resourceProps;
		resourceProps.mType = SR_ResourceType::Texture2D;
		resourceProps.mFormat = SR_Format::RG11B10_FLOAT;
		resourceProps.mSize = SC_IntVector(SR_RenderDevice::gInstance->GetSwapChain()->GetProperties().mSize, 1);
		resourceProps.mAllowUnorderedAccess = true;
		resourceProps.mDebugName = "Reflections";
		SC_Ref<SR_TextureResource> rtrResource = SR_RenderDevice::gInstance->CreateTextureResource(resourceProps);

		SR_TextureProperties textureProps(resourceProps.mFormat);
		mReflectionsTexture = SR_RenderDevice::gInstance->CreateTexture(textureProps, rtrResource);
		textureProps.mWritable = true;
		mReflectionsTextureRW = SR_RenderDevice::gInstance->CreateTexture(textureProps, rtrResource);
	}
	return true;
}

void SGfx_DDGI::UpdateResources()
{
	CreateTextures();
}

bool SGfx_DDGI::InitDebugResources()
{
	struct Vertex
	{
		SC_Vector mPosition;
		SC_Vector mNormal;
	};

	SC_Array<SC_Vector> vertices;
	SC_Array<uint32> indices;
	SGfx_Shapes::GenerateSphere(vertices, indices, 4, 0.125f);

	SC_Array<Vertex> sphereVertices;
	sphereVertices.Respace(vertices.Count());

	for (uint32 tri = 0; tri < indices.Count(); tri += 3)
	{
		SC_Vector verts[3] = { vertices[indices[tri]], vertices[indices[tri + 1]], vertices[indices[tri + 2]] };
		Vertex* sphereVerts[3] = { &sphereVertices[indices[tri]], &sphereVertices[indices[tri + 1]], &sphereVertices[indices[tri + 1]] };

		SC_Vector v0 = verts[1] - verts[0];
		SC_Vector v1 = verts[2] - verts[0];
		SC_Vector p = v0.Cross(v1);

		sphereVerts[0]->mNormal += p;
		sphereVerts[1]->mNormal += p;
		sphereVerts[2]->mNormal += p;
	}

	for (uint32 i = 0; i < sphereVertices.Count(); ++i)
	{
		Vertex& sphereVert = sphereVertices[i];

		sphereVert.mPosition = vertices[i];
		sphereVert.mNormal = -sphereVert.mNormal.GetNormalized();
	}

	SR_BufferResourceProperties vertexBufferResourceProps;
	vertexBufferResourceProps.mElementCount = sphereVertices.Count();
	vertexBufferResourceProps.mElementSize = sphereVertices.ElementStride();
	vertexBufferResourceProps.mBindFlags = SR_BufferBindFlag_VertexBuffer;
	mSphereVertexBuffer = SR_RenderDevice::gInstance->CreateBufferResource(vertexBufferResourceProps, sphereVertices.GetBuffer());

	SR_BufferResourceProperties indexBufferResourceProps;
	indexBufferResourceProps.mElementCount = indices.Count();
	indexBufferResourceProps.mElementSize = indices.ElementStride();
	indexBufferResourceProps.mBindFlags = SR_BufferBindFlag_IndexBuffer;
	mSphereIndexBuffer = SR_RenderDevice::gInstance->CreateBufferResource(indexBufferResourceProps, indices.GetBuffer());

	SR_ShaderCompileArgs compileArgs;
	compileArgs.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/Raytracing/DDGI_DebugVisualizeProbe.ssf";
	compileArgs.mEntryPoint = "MainVS";
	compileArgs.mType = SR_ShaderType::Vertex;

	SR_ShaderStateProperties debugShaderProps;
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, debugShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Vertex)], &debugShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Vertex)]))
		return false;

	compileArgs.mType = SR_ShaderType::Pixel;
	compileArgs.mEntryPoint = "MainPS";
	if (!SR_RenderDevice::gInstance->CompileShader(compileArgs, debugShaderProps.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Pixel)], &debugShaderProps.mShaderMetaDatas[static_cast<uint32>(SR_ShaderType::Pixel)]))
		return false;

	debugShaderProps.mVertexLayout.SetAttribute(SR_VertexAttribute::Position, SR_Format::RGB32_FLOAT);
	debugShaderProps.mVertexLayout.SetAttribute(SR_VertexAttribute::Normal, SR_Format::RGB32_FLOAT);
	debugShaderProps.mRTVFormats.mNumColorFormats = 1;
	debugShaderProps.mRTVFormats.mColorFormats[0] = SR_Format::RG11B10_FLOAT;
	debugShaderProps.mPrimitiveTopology = SR_PrimitiveTopology::TriangleList;
	debugShaderProps.mRasterizerProperties.mCounterClockWise = true;
	debugShaderProps.mDepthStencilProperties.mDepthComparisonFunc = SR_ComparisonFunc::GreaterEqual;
	debugShaderProps.mDepthStencilProperties.mWriteDepth = true;
	mDebugSphereShader = SR_RenderDevice::gInstance->CreateShaderState(debugShaderProps);

	return true;
}

SGfx_Raytracing::SGfx_Raytracing()
{

}

SGfx_Raytracing::~SGfx_Raytracing()
{

}

bool SGfx_Raytracing::Init()
{
	if (!mDDGI.Init())
		return false;

	if (!mReflections.Init())
		return false;

	return true;
}

SGfx_DDGI* SGfx_Raytracing::GetDDGI()
{
	return &mDDGI;
}

SGfx_Reflections* SGfx_Raytracing::GetReflections()
{
	return &mReflections;
}
