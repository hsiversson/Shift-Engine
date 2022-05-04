#include "SGfx_MaterialNode_TextureSample.h"
#include "SGfx_MaterialCompiler.h"

SGfx_MaterialNode_TextureSample::SGfx_MaterialNode_TextureSample()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));

	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_TextureSample::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* inTexture = mInputs[InTexture].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* inSampler = mInputs[InSampler].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* inUV = mInputs[InUV].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* inMip = mInputs[InMip].Read<SR_ShaderVariableProperties>();

	bool isRGBOutputConnected = mOutputs[OutRGB].IsConnected();
	bool isROutputConnected = mOutputs[OutR].IsConnected();
	bool isGOutputConnected = mOutputs[OutG].IsConnected();
	bool isBOutputConnected = mOutputs[OutB].IsConnected();
	bool isAOutputConnected = mOutputs[OutA].IsConnected();

	if (isRGBOutputConnected || isROutputConnected || isGOutputConnected || isBOutputConnected || isAOutputConnected)
	{
		std::string sampleFuncCode;
		if (mInputs[InMip].IsConnected())
			sampleFuncCode = SC_FormatStr("SampleLevel({}, {}, {})", inSampler->mName.c_str(), inUV->mName.c_str(), inMip->mName.c_str());
		else
			sampleFuncCode = SC_FormatStr("Sample({}, {})", inSampler->mName.c_str(), inUV->mName.c_str());

		std::string code = SC_FormatStr(
			"float4 textureSample_{} = {}.{};\n",
			mNodeId,
			inTexture->mName.c_str(),
			sampleFuncCode.c_str()
		);
		aCompiler->AddCode(code);
	}


	if (isRGBOutputConnected)
	{
		SR_ShaderVariableProperties outVariable;
		outVariable.mType = "float3";
		outVariable.mName = SC_FormatStr("textureSampleRGB_{}", mNodeId);
		std::string code = SC_FormatStr(
			"{} {} = textureSample_{}.rgb;\n",
			outVariable.mType.c_str(),
			outVariable.mName.c_str(),
			mNodeId
		);
		aCompiler->AddCode(code);
		mOutputs[OutRGB].Write(outVariable);
	}
	if (isROutputConnected)
	{
		SR_ShaderVariableProperties outVariable;
		outVariable.mType = "float";
		outVariable.mName = SC_FormatStr("textureSampleR_{}", mNodeId);
		std::string code = SC_FormatStr(
			"{} {} = textureSample_{}.r;\n",
			outVariable.mType.c_str(),
			outVariable.mName.c_str(),
			mNodeId
		);
		aCompiler->AddCode(code);
		mOutputs[OutR].Write(outVariable);
	}
	if (isGOutputConnected)
	{
		SR_ShaderVariableProperties outVariable;
		outVariable.mType = "float";
		outVariable.mName = SC_FormatStr("textureSampleG_{}", mNodeId);
		std::string code = SC_FormatStr(
			"{} {} = textureSample_{}.g;\n",
			outVariable.mType.c_str(),
			outVariable.mName.c_str(),
			mNodeId
		);
		aCompiler->AddCode(code);
		mOutputs[OutG].Write(outVariable);
	}
	if (isBOutputConnected)
	{
		SR_ShaderVariableProperties outVariable;
		outVariable.mType = "float";
		outVariable.mName = SC_FormatStr("textureSampleB_{}", mNodeId);
		std::string code = SC_FormatStr(
			"{} {} = textureSample_{}.b;\n",
			outVariable.mType.c_str(),
			outVariable.mName.c_str(),
			mNodeId
		);
		aCompiler->AddCode(code);
		mOutputs[OutB].Write(outVariable);
	}
	if (isAOutputConnected)
	{
		SR_ShaderVariableProperties outVariable;
		outVariable.mType = "float";
		outVariable.mName = SC_FormatStr("textureSampleA_{}", mNodeId);
		std::string code = SC_FormatStr(
			"{} {} = textureSample_{}.a;\n",
			outVariable.mType.c_str(),
			outVariable.mName.c_str(),
			mNodeId
		);
		aCompiler->AddCode(code);
		mOutputs[OutA].Write(outVariable);
	}

	return true;
}
