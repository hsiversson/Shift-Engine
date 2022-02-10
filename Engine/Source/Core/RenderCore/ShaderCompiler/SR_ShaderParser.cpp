#include "SR_ShaderParser.h"


#define PEEK_CHAR(aString, aCharsRemaining) ((aCharsRemaining) ? *(aString) : 0)
#define PEEK_CHAR_N(aString, aCharsRemaining, aIndex) (((aCharsRemaining) > (aIndex)) ? (aString)[(aIndex)] : 0)
#define SKIP_CHAR(aString, aCharsRemaining) ++(aString), --(aCharsRemaining)

SR_ShaderParser::SR_ShaderParser()
{

}

SR_ShaderParser::~SR_ShaderParser()
{

}

bool SR_ShaderParser::ParseFile(const SC_FilePath& /*aFilePath*/)
{
	return true;
}

bool SR_ShaderParser::ParseString(const std::string& /*aString*/)
{
	return true;
}

const SR_ShaderGenerationProperties& SR_ShaderParser::GetShaderGenerationProperties() const
{
	return mShaderProps;
}

bool SR_ShaderParser::Parse(const char*& aString, uint32& aCharsRemaining)
{
	if (!IgnoreSpace(aString, aCharsRemaining))
		return PostError();

	for (;;)
	{
		if (!IgnoreSpace(aString, aCharsRemaining))
			return PostError();

		if (!aCharsRemaining)
			break;

		//if (RequireChar(aString, aCharsRemaining, ';'))
		//	return PostError(/*aText, aRemaining, "unexpected ';'"*/);
		//else if (RequireString(aString, aCharsRemaining, "include"))
		//{
		//	if (!ProcessIncludes(aString, aCharsRemaining))
		//		return false;
		//}
		//else
		//{
		//	if (!ProcessGeneric(aString, aCharsRemaining))
		//		return false;
		//}
	}

	return true;
}

bool SR_ShaderParser::ProcessGeneric(const char*& /*aString*/, uint32& /*aCharsRemaining*/)
{
	//char c = PEEK_CHAR(aString, aCharsRemaining);
	//if (c == 's' && RequireString(aString, aCharsRemaining, "struct"))
	//{
	//	SR_ShaderStructDeclaration structDecl;
	//	if (!ParseStruct(aString, aCharsRemaining, structDecl))
	//		return false;
	//
	//	mShaderProps.mDeclarations.mStructs.Add(structDecl);
	//	return true;
	//}
	//else if (c == 'c' && RequireString(aString, aCharsRemaining, "const"))
	//{
	//	if (!RequireSpace(aString, aCharsRemaining))
	//	{
	//
	//	}
	//}


	return false;
}

bool SR_ShaderParser::PreProcess(const char*& /*aString*/, uint32& /*aCharsRemaining*/)
{
	return true;
}

bool SR_ShaderParser::ProcessIncludes(const char*& /*aString*/, uint32& /*aCharsRemaining*/)
{
	return true;
}

bool SR_ShaderParser::ProcessComments(const char*& /*aString*/, uint32& /*aCharsRemaining*/)
{
	return true;
}

bool SR_ShaderParser::ProcessResourceBindings(const char*& /*aString*/, uint32& /*aCharsRemaining*/)
{
	return true;
}

bool SR_ShaderParser::ProcessComputeShaderProperties(const char*& /*aString*/, uint32& /*aCharsRemaining*/)
{
	return true;
}

bool SR_ShaderParser::IgnoreSpace(const char*& /*aString*/, uint32& /*aCharsRemaining*/)
{
	return false;
}

bool SR_ShaderParser::PostError()
{
	return true;
}

bool SR_ShaderParser::PostWarning()
{
	return true;
}
