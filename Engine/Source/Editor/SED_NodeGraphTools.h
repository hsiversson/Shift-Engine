#pragma once
namespace SED_NodeGraphTools
{
	bool CreateContext();
	void DestroyContext();

	bool BeginEditor(const char* aId, const SC_Vector2& aSize);
	void EndEditor();

	bool BeginNode();
	void EndNode();

	bool BeginPin();
	void EndPin();

};

