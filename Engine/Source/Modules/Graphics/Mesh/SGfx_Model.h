#pragma once

class SC_FilePath;
class SGfx_MeshInstance;

/*
*
*	A collection of meshes that visually represents a object.
* 
*/

class SGfx_Model
{
public:
	SGfx_Model();
	~SGfx_Model();

	bool Init(const SC_FilePath& aModelFile);

	const SC_Array<SC_Ref<SGfx_MeshInstance>>& GetMeshes() const;

private:

	bool InitFromInternalSource(const SC_FilePath& aModelFile);
#if IS_EDITOR_BUILD
	bool InitFromExternalSource(const SC_FilePath& aModelFile);
#endif

	SC_Array<SC_Ref<SGfx_MeshInstance>> mMeshes;
};

