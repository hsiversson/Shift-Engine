#pragma once

class SC_FilePath;
class SGfx_MeshInstance;
class SGfx_MaterialInstance;

/*
*
*	A collection of meshes that visually represents a object.
* 
*/

struct SGfx_ModelSubMesh
{
	SC_Ref<SGfx_MeshInstance> mMesh;
	SC_Ref<SGfx_MaterialInstance> mMaterial;
};

class SGfx_Model
{
public:
	SGfx_Model();
	~SGfx_Model();

	bool Init(const SC_FilePath& aModelFile);

	uint32 GetNumMeshes() const;
	const SC_Array<SGfx_ModelSubMesh>& GetMeshes() const;

private:

	SC_Array<SGfx_ModelSubMesh> mMeshes;
};

