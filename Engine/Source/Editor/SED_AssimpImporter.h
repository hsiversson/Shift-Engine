#pragma once
#include "Graphics/Mesh/SGfx_Mesh.h"
#include "Graphics/Material/SGfx_Material.h"

namespace Assimp { class Importer; }
struct aiScene;
struct aiMaterial;
struct aiMesh;
struct aiNode;
class SGF_Level;
class SGfx_Material;
class SGfx_Mesh;

class SED_AssimpMaterial
{
	friend class SED_AssimpScene;
public:
	SED_AssimpMaterial();
	~SED_AssimpMaterial();

	bool SaveToDisk(const SC_FilePath& aFileDirectory) const;
	SC_Ref<SGfx_Material> GetMaterial() const;

private:
	SED_AssimpMaterial(aiMaterial* aMaterial, const SC_FilePath& aSourceFile);

	SGfx_MaterialProperties mMaterialProperties;
	aiMaterial* mImportedMaterial;
	SC_FilePath mSourceDir;
};

class SED_AssimpMesh
{
	friend class SED_AssimpScene;
public:
	SED_AssimpMesh();
	~SED_AssimpMesh();

	SC_Ref<SGfx_Mesh> GetMesh() const;
	bool SaveToDisk(const SC_FilePath& aFileDirectory) const;

	uint32 GetMaterialIndex() const;
	const SC_FilePath& GetSavePath() const;

	const SC_Vector& GetWorldOriginOffset() const;

private:
	SED_AssimpMesh(aiMesh* aMesh, uint32 aMaterialIndex, const SC_FilePath& aSourceFile);

	void ExtractVertices(SGfx_MeshCreateParams& aOutCreateParams, SC_Vector& aOutWorldOriginCenterOffset) const;
	void ExtractIndices(SGfx_MeshCreateParams& aOutCreateParams) const;
	bool GenerateMeshlets(SGfx_MeshCreateParams& aOutCreateParams) const;

	mutable SGfx_MeshCreateParams mMeshParams;
	aiMesh* mImportedMesh;
	uint32 mMaterialIndex;
	SC_FilePath mSourceDir;
	SC_Vector mWorldOriginOffset;
};

class SED_AssimpScene
{
	friend class SED_AssimpImporter;
public:
	SED_AssimpScene();
	~SED_AssimpScene();

	bool Init(const SC_FilePath& aSourceFile);

	uint32 GetNumMeshes() const;
	uint32 GetNumMaterials() const;

	bool ConvertToLevelAndSave(SGF_Level& aOutLevel);

private:
	void VisitNode(aiNode* aNode, SGF_Level& aOutLevel);

	SC_Array<SED_AssimpMaterial> mMaterials;
	SC_Array<SED_AssimpMesh> mMeshes;

	SC_FilePath mSourceFile;

	SC_UniquePtr<aiScene> mImportedScene;
};

class SED_AssimpImporter
{
public:
	SED_AssimpImporter();
	~SED_AssimpImporter();

	bool ImportScene(const SC_FilePath& aFilePath, SED_AssimpScene& aOutScene, float aImportScale = 1.0f);

private:
	SC_UniquePtr<Assimp::Importer> mImporter;
};

