#include "SGfx_Model.h"
#include "SGfx_MeshInstance.h"
#include "SGfx_MeshLoader.h"

#if IS_EDITOR_BUILD
#include "SGfx_ModelImporter.h"
#endif
#include "Graphics/Material/SGfx_MaterialInstance.h"

enum class SGfx_ModelFileType
{
	SMF,
	EXTERNAL,
	UNKNOWN,
};

static SGfx_ModelFileType GetModelFileType(const char* aFileExtension)
{
	if (strcmp(aFileExtension, SGfx_MeshLoader::gFileExtension) == 0)
		return SGfx_ModelFileType::SMF;
	else if (strcmp(aFileExtension, ".fbx") == 0 ||
			 strcmp(aFileExtension, ".3ds") == 0 ||
			 strcmp(aFileExtension, ".obj") == 0)
		return SGfx_ModelFileType::EXTERNAL;
	else
		return SGfx_ModelFileType::UNKNOWN;
}

SGfx_Model::SGfx_Model()
{

}

SGfx_Model::~SGfx_Model()
{

}

bool SGfx_Model::Init(const SC_FilePath& aModelFile)
{
	SGfx_ModelFileType fileType = GetModelFileType(aModelFile.GetFileExtension().c_str());

	if (fileType == SGfx_ModelFileType::SMF)
		return InitFromInternalSource(aModelFile);
#if IS_EDITOR_BUILD
	else if (fileType == SGfx_ModelFileType::EXTERNAL)
		return InitFromExternalSource(aModelFile);
#endif

	// LOG("Invalid model file type provided.");
	return false;
}

const SC_Array<SC_Ref<SGfx_MeshInstance>>& SGfx_Model::GetMeshes() const
{
	return mMeshes;
}

bool SGfx_Model::InitFromInternalSource(const SC_FilePath& /*aModelFile*/)
{
	SC_Array<SGfx_MeshCreateParams> meshCreateParams;
	//if (!SGfx_MeshLoader::Load(aModelFile, meshCreateParams))
	//	return false;

	if (SR_RenderDevice::gInstance->GetSupportCaps().mEnableMeshShaders)
	{
		
	}
	else
	{
		// Convert to index buffer to be used with standard geometry pipeline
	}

	for (const SGfx_MeshCreateParams& createParam : meshCreateParams)
	{
		SC_Ref<SGfx_MeshInstance> mesh = SGfx_MeshInstance::Create(createParam);
		if (mesh)
		{
			mesh->SetMaterialInstance(SGfx_MaterialInstance::GetDefault());
			mMeshes.Add(mesh);
		}
	}

	return true;
}

#if IS_EDITOR_BUILD
bool SGfx_Model::InitFromExternalSource(const SC_FilePath& aModelFile)
{
	SGfx_ModelImporter importer;
	SC_Ref<SGfx_ModelImporterScene> scene = importer.ImportScene(aModelFile.GetAbsolutePath().c_str());

	SC_Array<SGfx_MeshCreateParams> meshCreateParams;
	scene->Import(meshCreateParams);

	for (const SGfx_MeshCreateParams& createParam : meshCreateParams)
	{
		SC_Ref<SGfx_MeshInstance> mesh = SGfx_MeshInstance::Create(createParam);
		if (mesh)
		{
			mesh->SetMaterialInstance(SGfx_MaterialInstance::GetDefault());
			mMeshes.Add(mesh);
		}
	}

	return true;
}
#endif
