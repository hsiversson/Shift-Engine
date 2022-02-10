#pragma once
#include "RenderCore/Defines/SR_Semantics.h"

enum SR_ShaderVariableFlags
{
	SR_ShaderVariableFlag_In				= 0x0001,
	SR_ShaderVariableFlag_Out				= 0x0002,
	SR_ShaderVariableFlag_InOut				= SR_ShaderVariableFlag_In | SR_ShaderVariableFlag_Out,
	SR_ShaderVariableFlag_Centroid			= 0x0004,
	SR_ShaderVariableFlag_NoInterpolation	= 0x0008,
	SR_ShaderVariableFlag_Precise			= 0x0010,
	SR_ShaderVariableFlag_LDS				= 0x0020,
};

struct SR_ShaderVariableProperties
{
	std::string mType;
	std::string mName;

	SR_Semantic::Semantic mSemantic;
	uint32 mSemanticIndex;
	uint32 mFlags;
};

struct SR_ShaderIncludeReference
{
	SR_ShaderIncludeReference(const SC_FilePath& aIncludePath) { mIncludePath = aIncludePath; }
	SR_ShaderIncludeReference(const char* aIncludePath) { mIncludePath = aIncludePath; }

	SC_FilePath mIncludePath;
};

struct SR_ShaderStructDeclaration
{
	std::string mName;
	SC_Array<SR_ShaderVariableProperties> mMemberVariables;
};

struct SR_ShaderResourceDeclaration
{

};

struct SR_ShaderFunctionDeclaration
{
	SC_Array<SR_ShaderVariableProperties> mArguments;
	std::string mReturnType;
	std::string mFunctionName;
	std::string mCode;
};

struct SR_ShaderDeclarationProperties
{
	SC_Array<SR_ShaderIncludeReference> mIncludes;
	SC_Array<SR_ShaderStructDeclaration> mStructs;
	SC_Array<SR_ShaderResourceDeclaration> mResourceBindings;
	SC_Array<SR_ShaderFunctionDeclaration> myFunctions;
	//SC_Array<MR_ShaderVariableProperties> myLiterals;
	//SC_Array<MR_ShaderVariableProperties> myGlobals;
};

struct SR_ShaderGenerationProperties
{
	SR_ShaderDeclarationProperties mDeclarations;
	SC_Array<SR_ShaderVariableProperties> mInputs;
	SC_Array<SR_ShaderVariableProperties> mOutputs;

	SC_Array<SR_ShaderVariableProperties> mMainFuncArgs;
	std::string mMainCode;

	SR_ShaderType mShaderType;

};