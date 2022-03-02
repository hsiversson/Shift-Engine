#pragma once
#include "SGfx_Mesh.h"

class SGfx_Model;
class SGfx_ModelLoader
{
public:
	static constexpr const char* gFileExtension = ".smdl";

	static bool Save(const SC_FilePath& aFilePath, const SGfx_Model& aModel);
	static bool Load(const SC_FilePath& aFilePath, SGfx_Model& aOutModel);

private:
	static constexpr uint32 gCurrentSMDLVersion = 1;
};

