#include "SGfx_Model.h"
#include "SGfx_MeshInstance.h"
#include "SGfx_MeshLoader.h"

#include "Graphics/Material/SGfx_MaterialInstance.h"

SGfx_Model::SGfx_Model()
{

}

SGfx_Model::~SGfx_Model()
{

}

bool SGfx_Model::Init(const SC_FilePath& /*aModelFile*/)
{


	return false;
}

uint32 SGfx_Model::GetNumMeshes() const
{
	return mMeshes.Count();
}

const SC_Array<SGfx_ModelSubMesh>& SGfx_Model::GetMeshes() const
{
	return mMeshes;
}
