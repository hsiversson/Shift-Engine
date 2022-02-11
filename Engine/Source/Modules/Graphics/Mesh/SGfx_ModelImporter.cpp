#include "SGfx_ModelImporter.h"

#if IS_EDITOR_BUILD

#include "SGfx_Mesh.h"
#include "SGfx_MeshletGenerator.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <filesystem>
#include <fstream>

//#include "zlib.h"
#include "SGfx_MeshLoader.h"

SGfx_ModelImporterScene::SGfx_ModelImporterScene()
{

}

SGfx_ModelImporterScene::~SGfx_ModelImporterScene()
{

}

bool SGfx_ModelImporterScene::HasMeshes() const
{
	return mImportedScene->HasMeshes();
}

uint32 SGfx_ModelImporterScene::GetNumMeshes() const
{
	return mImportedScene->mNumMeshes;
}

bool SGfx_ModelImporterScene::Import(SC_Array<SGfx_MeshCreateParams>& aOutCreateParams)
{
	aOutCreateParams.Reserve(mImportedScene->mNumMeshes);
	for (uint32 meshIdx = 0; meshIdx < mImportedScene->mNumMeshes; ++meshIdx)
	{
		const aiMesh* mesh = mImportedScene->mMeshes[meshIdx];

		SR_VertexLayout vertexLayout;
		vertexLayout.SetAttribute(SR_VertexAttribute::Position, SR_Format::RGB32_FLOAT);
		if (!vertexLayout.HasAttribute(SR_VertexAttribute::Position))
		{
			assert(false && "Mesh is missing positions, cannot continue processing.");
			continue;
		}

		if (mesh->HasNormals())
			vertexLayout.SetAttribute(SR_VertexAttribute::Normal,SR_Format::RGB32_FLOAT);

		if (mesh->HasTangentsAndBitangents())
		{
			vertexLayout.SetAttribute(SR_VertexAttribute::Tangent, SR_Format::RGB32_FLOAT);
			vertexLayout.SetAttribute(SR_VertexAttribute::Bitangent, SR_Format::RGB32_FLOAT);
		}

		if (mesh->HasTextureCoords(0))
			vertexLayout.SetAttribute(SR_VertexAttribute::UV0, SR_Format::RG32_FLOAT);
		if (mesh->HasTextureCoords(1))
			vertexLayout.SetAttribute(SR_VertexAttribute::UV1, SR_Format::RG32_FLOAT);

		if (mesh->HasVertexColors(0))
			vertexLayout.SetAttribute(SR_VertexAttribute::Color0, SR_Format::RGBA32_FLOAT);
		if (mesh->HasVertexColors(1))
			vertexLayout.SetAttribute(SR_VertexAttribute::Color1, SR_Format::RGBA32_FLOAT);

		SGfx_MeshCreateParams& params = aOutCreateParams.Add();
		params.mVertexLayout = vertexLayout;
		ExtractVertices(mesh, params);
		ExtractIndices(mesh, params);

		GenerateMaterialParams(mesh);

		GenerateMeshlets(params);
		params.mIsMeshletData = true;
	}

	//SGfx_MeshLoader::Save("D:/Programmering/hsiversson_HAMPUS_WKS_Project_Iris_Main/Iris/Data/Models/SunTemple/SunTemple.smf", aOutCreateParams);
	return true;
}

void SGfx_ModelImporterScene::ExtractVertices(const aiMesh* aMesh, SGfx_MeshCreateParams& aOutCreateParams)
{
	SR_VertexLayout& vertexLayout = aOutCreateParams.mVertexLayout;
	const uint32 vertexStrideSize = vertexLayout.GetVertexStrideSize();
	const uint32 vertexDataArraySize = aMesh->mNumVertices * vertexStrideSize;
	aOutCreateParams.mVertexData.Respace(vertexDataArraySize);
	uint32 currentDataArrayPos = 0;

	aOutCreateParams.mAABBMin = SC_Vector(SC_FLT_MAX);
	aOutCreateParams.mAABBMax = SC_Vector(SC_FLT_LOWEST);
	for (uint32 i = 0; i < aMesh->mNumVertices; ++i)
	{
		SC_Vector vPosition = SC_Vector(aMesh->mVertices[i].x, aMesh->mVertices[i].y, aMesh->mVertices[i].z) * 0.01f;

		SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vPosition, sizeof(SC_Vector));
		currentDataArrayPos += sizeof(SC_Vector);

		if (vertexLayout.HasAttribute(SR_VertexAttribute::Normal))
		{
			SC_Vector vNormal = SC_Vector(aMesh->mNormals[i].x, aMesh->mNormals[i].y, aMesh->mNormals[i].z);
			vNormal.Normalize();
			SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vNormal, sizeof(SC_Vector));
			currentDataArrayPos += sizeof(SC_Vector);
		}

		if (vertexLayout.HasAttribute(SR_VertexAttribute::Tangent))
		{
			SC_Vector vTangentAndBitangent[2];
			vTangentAndBitangent[0] = SC_Vector(aMesh->mTangents[i].x, aMesh->mTangents[i].y, aMesh->mTangents[i].z);
			vTangentAndBitangent[0].Normalize();
			vTangentAndBitangent[1] = SC_Vector(aMesh->mBitangents[i].x, aMesh->mBitangents[i].y, aMesh->mBitangents[i].z);
			vTangentAndBitangent[1].Normalize();
			SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vTangentAndBitangent, (sizeof(SC_Vector) * 2));
			currentDataArrayPos += (sizeof(SC_Vector) * 2);
		}

		if (vertexLayout.HasAttribute(SR_VertexAttribute::UV0))
		{
			SC_Vector2 vUV = SC_Vector2(aMesh->mTextureCoords[0][i].x, aMesh->mTextureCoords[0][i].y);
			SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vUV, sizeof(SC_Vector2));
			currentDataArrayPos += sizeof(SC_Vector2);
		}

		if (vertexLayout.HasAttribute(SR_VertexAttribute::UV1))
		{
			SC_Vector2 vUV = SC_Vector2(aMesh->mTextureCoords[1][i].x, aMesh->mTextureCoords[1][i].y);
			SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vUV, sizeof(SC_Vector2));
			currentDataArrayPos += sizeof(SC_Vector2);
		}

		if (vertexLayout.HasAttribute(SR_VertexAttribute::Color0))
		{
			SC_Vector4 vColor = SC_Vector4(aMesh->mColors[0][i].r, aMesh->mColors[0][i].g, aMesh->mColors[0][i].b, aMesh->mColors[0][i].a);
			SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vColor, sizeof(SC_Vector4));
			currentDataArrayPos += sizeof(SC_Vector4);
		}

		if (vertexLayout.HasAttribute(SR_VertexAttribute::Color1))
		{
			SC_Vector4 vColor = SC_Vector4(aMesh->mColors[1][i].r, aMesh->mColors[1][i].g, aMesh->mColors[1][i].b, aMesh->mColors[1][i].a);
			SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vColor, sizeof(SC_Vector4));
			currentDataArrayPos += sizeof(SC_Vector4);
		}

		aOutCreateParams.mAABBMin.x = SC_Min(vPosition.x, aOutCreateParams.mAABBMin.x);
		aOutCreateParams.mAABBMin.y = SC_Min(vPosition.y, aOutCreateParams.mAABBMin.y);
		aOutCreateParams.mAABBMin.z = SC_Min(vPosition.z, aOutCreateParams.mAABBMin.z);
		aOutCreateParams.mAABBMax.x = SC_Max(vPosition.x, aOutCreateParams.mAABBMax.x);
		aOutCreateParams.mAABBMax.y = SC_Max(vPosition.y, aOutCreateParams.mAABBMax.y);
		aOutCreateParams.mAABBMax.z = SC_Max(vPosition.z, aOutCreateParams.mAABBMax.z);
	}
}

void SGfx_ModelImporterScene::ExtractIndices(const aiMesh* aMesh, SGfx_MeshCreateParams& aOutCreateParams)
{
	const uint32 numIndices = aMesh->mNumFaces * 3;
	const bool use16BitIndices = (numIndices < SC_UINT16_MAX);
	aOutCreateParams.mIndexStride = (use16BitIndices) ? sizeof(uint16) : sizeof(uint32);
	aOutCreateParams.mIndexData.Respace(numIndices * aOutCreateParams.mIndexStride);

	uint32 currentDataArrayPos = 0;
	for (uint32 faceIdx = 0; faceIdx < aMesh->mNumFaces; ++faceIdx)
	{
		const aiFace& face = aMesh->mFaces[faceIdx];
		assert(face.mNumIndices == 3 && "Mesh isn't triangulated.");

		
		for (uint32 i = 0; i < 3; ++i)
		{
			if (use16BitIndices)
			{
				uint16* index = reinterpret_cast<uint16*>(&aOutCreateParams.mIndexData[currentDataArrayPos]);
				*index = static_cast<uint16>(face.mIndices[i]);
			}
			else
			{
				uint32* index = reinterpret_cast<uint32*>(&aOutCreateParams.mIndexData[currentDataArrayPos]);
				*index = static_cast<uint32>(face.mIndices[i]);
			}
			currentDataArrayPos += aOutCreateParams.mIndexStride;
		}
	}
}

void SGfx_ModelImporterScene::GenerateMaterialParams(const aiMesh* aMesh)
{
	const aiMaterial* material = mImportedScene->mMaterials[aMesh->mMaterialIndex];

	aiString path;
	for (uint32 i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
	{
		aiReturn texFound = material->GetTexture((aiTextureType)i, 0, &path);
		if (texFound != AI_SUCCESS)
			continue;

		std::string texturePath = path.data;

		//textures.Add(texturePath);
	}
}

bool SGfx_ModelImporterScene::GenerateMeshlets(SGfx_MeshCreateParams& aCreateParams)
{
	const uint32 numVertices = aCreateParams.mVertexData.Count() / aCreateParams.mVertexLayout.GetVertexStrideSize();
	const uint32 numIndices = aCreateParams.mIndexData.Count() / aCreateParams.mIndexStride;

	bool result = SGfx_GenerateMeshlets(
		64, 
		64, 
		aCreateParams.mVertexData.GetBuffer(),
		aCreateParams.mVertexLayout,
		numVertices,
		aCreateParams.mIndexData.GetBuffer(),
		(aCreateParams.mIndexStride == sizeof(uint16)) ? true:false, 
		numIndices,
		aCreateParams.mMeshlets,
		aCreateParams.mPrimitiveIndices,
		aCreateParams.mVertexIndices);

	aCreateParams.mVertexIndicesStride = aCreateParams.mIndexStride;
	return result;
}

void SGfx_ModelImporterScene::BuildScene()
{


}

SGfx_ModelImporter::SGfx_ModelImporter()
{
	mImporter = SC_MakeUnique<Assimp::Importer>();
}

SGfx_ModelImporter::~SGfx_ModelImporter()
{

}

SC_Ref<SGfx_ModelImporterScene> SGfx_ModelImporter::ImportScene(const char* aModelFile)
{
	SC_Ref<SGfx_ModelImporterScene> scene;

	mImporter->SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 0.01f);

	uint32 flags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded | aiProcess_TransformUVCoords | aiProcess_OptimizeGraph | aiProcess_GlobalScale;
	const aiScene* importedScene = mImporter->ReadFile(aModelFile, flags); // Absolute path?

	if (!importedScene)
		return scene;

	scene = SC_MakeRef<SGfx_ModelImporterScene>();
	scene->mImportedScene.reset(mImporter->GetOrphanedScene());
	return scene;
}

#endif

