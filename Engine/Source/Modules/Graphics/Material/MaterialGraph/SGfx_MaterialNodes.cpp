#include "SGfx_MaterialNodes.h"
#include "SGfx_MaterialCompiler.h"

SGfx_MaterialNode_Constant::SGfx_MaterialNode_Constant()
{
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Constant::Compile(SGfx_MaterialCompiler* aCompiler)
{
	std::string type;
	std::string value;
	switch (mType)
	{
	case Type::Bool:
		type = "bool";
		value = (mValue.mBool) ? "true" : "false";
		break;
	case Type::Float:
		type = "float";
		value = SC_FormatStr("%f", mValue.mFloat4.x);
		break;
	case Type::Float2:
		type = "float2";
		value = SC_FormatStr("float2(%f, %f)", mValue.mFloat4.x, mValue.mFloat4.y);
		break;
	case Type::Float3:
		type = "float3";
		value = SC_FormatStr("float3(%f, %f, %f)", mValue.mFloat4.x, mValue.mFloat4.y, mValue.mFloat4.z);
		break;
	case Type::Float4:
		type = "float4";
		value = SC_FormatStr("float4(%f, %f, %f, %f)", mValue.mFloat4.x, mValue.mFloat4.y, mValue.mFloat4.z, mValue.mFloat4.w);
		break;
	default:
		SC_ASSERT(false, "Invalid type");
		return false;
	}

	mVariableProps.mName = SC_FormatStr("constant_%s_%i", type.c_str(), mNodeId);
	mVariableProps.mType = type;

	std::string code = SC_FormatStr("%s %s = %s;\n", mVariableProps.mType.c_str(), mVariableProps.mName.c_str(), value.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(mVariableProps);
	return true;
}

void SGfx_MaterialNode_Constant::SetValue(bool aValue)
{
	mType = Type::Bool;
	mValue.mBool = aValue;
}

void SGfx_MaterialNode_Constant::SetValue(float aValue)
{
	mType = Type::Float;
	mValue.mFloat = aValue;
}

void SGfx_MaterialNode_Constant::SetValue(const SC_Vector2& aValue)
{
	mType = Type::Float2;
	mValue.mFloat2 = aValue;
}

void SGfx_MaterialNode_Constant::SetValue(const SC_Vector& aValue)
{
	mType = Type::Float3;
	mValue.mFloat3 = aValue;
}

void SGfx_MaterialNode_Constant::SetValue(const SC_Vector4& aValue)
{
	mType = Type::Float4;
	mValue.mFloat4 = aValue;
}

SGfx_MaterialNode_Add::SGfx_MaterialNode_Add()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Add::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* in1 = mInputs[In1].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* in2 = mInputs[In2].Read<SR_ShaderVariableProperties>();

	const std::string& type = in1->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("add_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = %s + %s;\n", outVariable.mType.c_str(), outVariable.mName.c_str(), in1->mName.c_str(), in2->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Subtract::SGfx_MaterialNode_Subtract()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Subtract::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* in1 = mInputs[In1].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* in2 = mInputs[In2].Read<SR_ShaderVariableProperties>();

	const std::string& type = in1->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("sub_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = %s - %s;\n", outVariable.mType.c_str(), outVariable.mName.c_str(), in1->mName.c_str(), in2->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Multiply::SGfx_MaterialNode_Multiply()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Multiply::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* in1 = mInputs[In1].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* in2 = mInputs[In2].Read<SR_ShaderVariableProperties>();

	const std::string& type = in1->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("mul_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = %s * %s;\n", outVariable.mType.c_str(), outVariable.mName.c_str(), in1->mName.c_str(), in2->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Divide::SGfx_MaterialNode_Divide()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Divide::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* in1 = mInputs[In1].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* in2 = mInputs[In2].Read<SR_ShaderVariableProperties>();

	const std::string& type = in1->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("div_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = %s / %s;\n", outVariable.mType.c_str(), outVariable.mName.c_str(), in1->mName.c_str(), in2->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Reciprocal::SGfx_MaterialNode_Reciprocal()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Reciprocal::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* input = mInputs[In].Read<SR_ShaderVariableProperties>();

	const std::string& type = input->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("rcp_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = rcp(%s);\n", outVariable.mType.c_str(), outVariable.mName.c_str(), input->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Abs::SGfx_MaterialNode_Abs()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Abs::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* input = mInputs[In].Read<SR_ShaderVariableProperties>();

	const std::string& type = input->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("abs_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = abs(%s);\n", outVariable.mType.c_str(), outVariable.mName.c_str(), input->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Length::SGfx_MaterialNode_Length()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Length::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* input = mInputs[In].Read<SR_ShaderVariableProperties>();

	const std::string& type = input->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("len_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = length(%s);\n", outVariable.mType.c_str(), outVariable.mName.c_str(), input->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Normalize::SGfx_MaterialNode_Normalize()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Normalize::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* input = mInputs[In].Read<SR_ShaderVariableProperties>();

	const std::string& type = input->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("normalize_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = normalize(%s);\n", outVariable.mType.c_str(), outVariable.mName.c_str(), input->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Saturate::SGfx_MaterialNode_Saturate()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Saturate::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* input = mInputs[In].Read<SR_ShaderVariableProperties>();

	const std::string& type = input->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("saturate_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = saturate(%s);\n", outVariable.mType.c_str(), outVariable.mName.c_str(), input->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Dot::SGfx_MaterialNode_Dot()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Dot::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* in1 = mInputs[In1].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* in2 = mInputs[In2].Read<SR_ShaderVariableProperties>();

	const std::string& type = in1->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("dot_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = dot(%s, %s);\n", outVariable.mType.c_str(), outVariable.mName.c_str(), in1->mName.c_str(), in2->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Min::SGfx_MaterialNode_Min()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Min::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* in1 = mInputs[In1].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* in2 = mInputs[In2].Read<SR_ShaderVariableProperties>();

	const std::string& type = in1->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("min_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = min(%s, %s);\n", outVariable.mType.c_str(), outVariable.mName.c_str(), in1->mName.c_str(), in2->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Max::SGfx_MaterialNode_Max()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Max::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* in1 = mInputs[In1].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* in2 = mInputs[In2].Read<SR_ShaderVariableProperties>();

	const std::string& type = in1->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("max_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = max(%s, %s);\n", outVariable.mType.c_str(), outVariable.mName.c_str(), in1->mName.c_str(), in2->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}

SGfx_MaterialNode_Clamp::SGfx_MaterialNode_Clamp()
{
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mInputs.Add(SGfx_MaterialNodeInputPin(this));
	mOutputs.Add(SGfx_MaterialNodeOutputPin(this));
}

bool SGfx_MaterialNode_Clamp::Compile(SGfx_MaterialCompiler* aCompiler)
{
	const SR_ShaderVariableProperties* inValue = mInputs[InValue].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* minClamp = mInputs[MinClamp].Read<SR_ShaderVariableProperties>();
	const SR_ShaderVariableProperties* maxClamp = mInputs[MaxClamp].Read<SR_ShaderVariableProperties>();

	const std::string& type = inValue->mType;

	SR_ShaderVariableProperties outVariable;
	outVariable.mType = type;
	outVariable.mName = SC_FormatStr("clamp_%i", mNodeId);

	std::string code = SC_FormatStr("%s %s = clamp(%s, %s, %s);\n", outVariable.mType.c_str(), outVariable.mName.c_str(), inValue->mName.c_str(), minClamp->mName.c_str(), maxClamp->mName.c_str());
	aCompiler->AddCode(code);

	mOutputs[Out].Write(outVariable);
	return true;
}
