#pragma once

#if IS_EDITOR_BUILD
#include "RenderCore/Interface/SR_VertexLayout.h"

struct aiScene;
struct aiMesh;

struct SGfx_MeshCreateParams;

class SGfx_ModelImporterScene : public SC_ReferenceCounted
{
	friend class SGfx_ModelImporter;
public:
	SGfx_ModelImporterScene();
	~SGfx_ModelImporterScene();

	bool HasMeshes() const;
	uint32 GetNumMeshes() const;

	bool Import(SC_Array<SGfx_MeshCreateParams>& aOutCreateParams);

private:
	void ExtractVertices(const aiMesh* aMesh, SGfx_MeshCreateParams& aOutCreateParams);
	void ExtractIndices(const aiMesh* aMesh, SGfx_MeshCreateParams& aOutCreateParams);

	void GenerateMaterialParams(const aiMesh* aMesh);

	bool GenerateMeshlets(SGfx_MeshCreateParams& aCreateParams);

	void BuildScene();

	SC_UniquePtr<aiScene> mImportedScene;
};

namespace Assimp
{
	class Importer;
}

class SGfx_ModelImporter
{
public:
	SGfx_ModelImporter();
	~SGfx_ModelImporter();

	SC_Ref<SGfx_ModelImporterScene> ImportScene(const char* aModelFile);

private:
	SC_UniquePtr<Assimp::Importer> mImporter;
};

#endif