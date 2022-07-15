#include "SED_AssimpImporter.h"
#include "Graphics/Mesh/SGfx_MeshletGenerator.h"
#include "Graphics/Mesh/SGfx_MeshLoader.h"
#include "Graphics/Mesh/SGfx_MeshInstance.h"
#include "GameFramework/Entity/SGF_Entity.h"
#include "GameFramework/Entity/Components/SGF_TransformComponent.h"
#include "GameFramework/Entity/Components/SGF_StaticMeshComponent.h"
#include "GameFramework/Entity/Components/SGF_EntityIdComponent.h"
#include "GameFramework/GameWorld/SGF_World.h"
#include "GameFramework/GameWorld/SGF_Level.h"
#include "RenderCore/ShaderCompiler/SR_DirectXShaderCompiler.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <filesystem>
#include <fstream>

SED_AssimpMaterial::SED_AssimpMaterial(aiMaterial* aMaterial, const SC_FilePath& aSourceFile)
	: mImportedMaterial(aMaterial)
	, mSourceDir(aSourceFile.GetParentDirectory())
{
	bool useAlphaTesting = false;

	aiString path;
	for (uint32 i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
	{
		aiTextureType texType = (aiTextureType)i;
		aiReturn texFound = mImportedMaterial->GetTexture((aiTextureType)i, 0, &path);
		if (texFound != AI_SUCCESS)
			continue;

		SC_FilePath texturePath(path.data);
		SC_FilePath truePath(mSourceDir + "/" + texturePath);
		if (SC_FilePath::Exists(truePath))
		{
			mMaterialProperties.mTextures.Add(truePath);
		}
		else
		{
			switch (texType)
			{
			case aiTextureType_DIFFUSE:
				mMaterialProperties.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Grey_1x1.dds");
				break;
			case aiTextureType_NORMALS:
				mMaterialProperties.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Normal_1x1.dds");
				break;
			case aiTextureType_DIFFUSE_ROUGHNESS:
				mMaterialProperties.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Grey_1x1.dds");
				break;
			case aiTextureType_METALNESS:
				mMaterialProperties.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Black_1x1.dds");
				break;
			case aiTextureType_AMBIENT_OCCLUSION:
				mMaterialProperties.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_White_1x1.dds");
				break;
			case aiTextureType_SPECULAR:
				mMaterialProperties.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_White_1x1.dds");
				break;
			}
		}

		if (texType == aiTextureType_OPACITY)
			useAlphaTesting = true;
	}

	mMaterialProperties.mShaderProperties.mRasterizerProperties.mCullMode = SR_CullMode::Back;
	mMaterialProperties.mShaderProperties.mBlendStateProperties.mNumRenderTargets = 1;
	mMaterialProperties.mShaderProperties.mRTVFormats.mNumColorFormats = 1;
	mMaterialProperties.mShaderProperties.mRTVFormats.mColorFormats[0] = SR_Format::RGBA8_UNORM;
	mMaterialProperties.mShaderProperties.mDepthStencilProperties.mWriteDepth = false;
	mMaterialProperties.mShaderProperties.mDepthStencilProperties.mDepthComparisonFunc = SR_ComparisonFunc::Equal;

	SR_DirectXShaderCompiler compiler;
	SR_ShaderCompileArgs args;
	args.mEntryPoint = "MainPS";
	args.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/DefaultMeshShader.ssf";
	args.mDefines.Add(SC_Pair<std::string, std::string>("PIXEL_SHADER", "1"));
	args.mType = SR_ShaderType::Pixel;
	compiler.CompileFromFile(args, mMaterialProperties.mShaderProperties.mShaderByteCodes[static_cast<uint32>(SR_ShaderType::Pixel)]);

	if (useAlphaTesting)
	{
		mMaterialProperties.mUseAlphaTesting = true;
		mMaterialProperties.mAlphaRef = 0.5f;
	}
	else
		mMaterialProperties.mUseAlphaTesting = false;

	mMaterialProperties.mOutputVelocity = true;
}

SED_AssimpMaterial::SED_AssimpMaterial()
	: mImportedMaterial(nullptr)
{
}

SED_AssimpMaterial::~SED_AssimpMaterial()
{

}

bool SED_AssimpMaterial::SaveToDisk(const SC_FilePath& aFileDirectory) const
{
	SC_FilePath path(aFileDirectory);
	SC_FilePath::CreateDirectory(path);

	path += std::string("/");
	path += std::string(mImportedMaterial->GetName().C_Str());
	path.RemoveExtension();
	path += std::string(".smf");

	//bool result = SGfx_MeshLoader::Save(path, mMeshParams);
	//
	//if (result)
	//	mMeshParams.mSourceFile = path;

	return true;
}

SC_Ref<SGfx_Material> SED_AssimpMaterial::GetMaterial() const
{
	return SC_MakeRef<SGfx_Material>(mMaterialProperties);
}

SED_AssimpMesh::SED_AssimpMesh(aiMesh* aMesh, uint32 aMaterialIndex, const SC_FilePath& aSourceFile)
	: mImportedMesh(aMesh)
	, mMaterialIndex(aMaterialIndex)
	, mSourceDir(aSourceFile.GetParentDirectory())
{
	SR_VertexLayout& vertexLayout = mMeshParams.mVertexLayout;
	vertexLayout.SetAttribute(SR_VertexAttribute::Position, SR_Format::RGB32_FLOAT);
	if (!vertexLayout.HasAttribute(SR_VertexAttribute::Position))
	{
		assert(false && "Mesh is missing positions, cannot continue processing.");
		return;
	}

	if (mImportedMesh->HasNormals())
		vertexLayout.SetAttribute(SR_VertexAttribute::Normal, SR_Format::RGB32_FLOAT);

	if (mImportedMesh->HasTangentsAndBitangents())
	{
		vertexLayout.SetAttribute(SR_VertexAttribute::Tangent, SR_Format::RGB32_FLOAT);
		vertexLayout.SetAttribute(SR_VertexAttribute::Bitangent, SR_Format::RGB32_FLOAT);
	}

	if (mImportedMesh->HasTextureCoords(0))
		vertexLayout.SetAttribute(SR_VertexAttribute::UV, SR_Format::RG32_FLOAT, 0);
	if (mImportedMesh->HasTextureCoords(1))
		vertexLayout.SetAttribute(SR_VertexAttribute::UV, SR_Format::RG32_FLOAT, 1);

	if (mImportedMesh->HasVertexColors(0))
		vertexLayout.SetAttribute(SR_VertexAttribute::Color, SR_Format::RGBA32_FLOAT, 0);
	if (mImportedMesh->HasVertexColors(1))
		vertexLayout.SetAttribute(SR_VertexAttribute::Color, SR_Format::RGBA32_FLOAT, 1);

	if (mImportedMesh->HasBones())
	{
		for (uint32 boneIndex = 0; boneIndex < mImportedMesh->mNumBones; ++boneIndex)
		{
			SR_VertexAttribute boneAttribute = static_cast<SR_VertexAttribute>(static_cast<uint32>(SR_VertexAttribute::BoneId) + boneIndex);
			SR_VertexAttribute boneWeightAttribute = static_cast<SR_VertexAttribute>(static_cast<uint32>(SR_VertexAttribute::BoneWeight) + boneIndex);
			vertexLayout.SetAttribute(boneAttribute, SR_Format::R32_UINT, boneIndex);
			vertexLayout.SetAttribute(boneWeightAttribute, SR_Format::R32_FLOAT, boneIndex);
		}
	}

	ExtractVertices(mMeshParams);
	ExtractIndices(mMeshParams);
	GenerateMeshlets(mMeshParams);
	mMeshParams.mIsMeshletData = true;
}

void SED_AssimpMesh::ExtractVertices(SGfx_MeshCreateParams& aOutCreateParams) const
{
	SR_VertexLayout& vertexLayout = aOutCreateParams.mVertexLayout;
	const SC_Array<SR_VertexAttributeData>& vertexAttributes = vertexLayout.mAttributes;

	const uint32 vertexStrideSize = vertexLayout.GetVertexStrideSize();
	const uint32 vertexDataArraySize = mImportedMesh->mNumVertices * vertexStrideSize;
	aOutCreateParams.mVertexData.Respace(vertexDataArraySize);
	uint32 currentDataArrayPos = 0;

	aOutCreateParams.mAABBMin = SC_Vector(SC_FLT_MAX);
	aOutCreateParams.mAABBMax = SC_Vector(SC_FLT_LOWEST);
	for (uint32 i = 0; i < mImportedMesh->mNumVertices; ++i)
	{
		for (const SR_VertexAttributeData& vAttribute : vertexAttributes)
		{
			if (vAttribute.mAttributeId == SR_VertexAttribute::Position)
			{
				SC_Vector vPosition = SC_Vector(mImportedMesh->mVertices[i].x, mImportedMesh->mVertices[i].y, mImportedMesh->mVertices[i].z);
				SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vPosition, sizeof(SC_Vector));
				currentDataArrayPos += sizeof(SC_Vector);

				aOutCreateParams.mAABBMin.x = SC_Min(vPosition.x, aOutCreateParams.mAABBMin.x);
				aOutCreateParams.mAABBMin.y = SC_Min(vPosition.y, aOutCreateParams.mAABBMin.y);
				aOutCreateParams.mAABBMin.z = SC_Min(vPosition.z, aOutCreateParams.mAABBMin.z);
				aOutCreateParams.mAABBMax.x = SC_Max(vPosition.x, aOutCreateParams.mAABBMax.x);
				aOutCreateParams.mAABBMax.y = SC_Max(vPosition.y, aOutCreateParams.mAABBMax.y);
				aOutCreateParams.mAABBMax.z = SC_Max(vPosition.z, aOutCreateParams.mAABBMax.z);
			}
			else if (vAttribute.mAttributeId == SR_VertexAttribute::Normal)
			{
				SC_Vector vNormal = SC_Vector(mImportedMesh->mNormals[i].x, mImportedMesh->mNormals[i].y, mImportedMesh->mNormals[i].z);
				vNormal.Normalize();
				SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vNormal, sizeof(SC_Vector));
				currentDataArrayPos += sizeof(SC_Vector);
			}
			else if (vAttribute.mAttributeId == SR_VertexAttribute::Tangent)
			{
				SC_Vector vTangent = SC_Vector(mImportedMesh->mTangents[i].x, mImportedMesh->mTangents[i].y, mImportedMesh->mTangents[i].z);
				vTangent.Normalize();
				SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vTangent, sizeof(SC_Vector));
				currentDataArrayPos += sizeof(SC_Vector);
			}
			else if (vAttribute.mAttributeId == SR_VertexAttribute::Bitangent)
			{
				SC_Vector vBitangent = SC_Vector(mImportedMesh->mBitangents[i].x, mImportedMesh->mBitangents[i].y, mImportedMesh->mBitangents[i].z);
				vBitangent.Normalize();
				SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vBitangent, sizeof(SC_Vector));
				currentDataArrayPos += sizeof(SC_Vector);
			}
			else if (vAttribute.mAttributeId == SR_VertexAttribute::UV)
			{
				SC_Vector4 vUV;
				vUV.x = mImportedMesh->mTextureCoords[vAttribute.mAttributeIndex][i].x;
				vUV.y = mImportedMesh->mTextureCoords[vAttribute.mAttributeIndex][i].y;
				SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vUV, sizeof(SC_Vector2));
				currentDataArrayPos += sizeof(SC_Vector2);
			}
			else if (vAttribute.mAttributeId == SR_VertexAttribute::Color)
			{
				SC_Vector4 vColor;
				vColor.x = mImportedMesh->mColors[vAttribute.mAttributeIndex][i].r;
				vColor.y = mImportedMesh->mColors[vAttribute.mAttributeIndex][i].g;
				vColor.z = mImportedMesh->mColors[vAttribute.mAttributeIndex][i].b;
				vColor.w = mImportedMesh->mColors[vAttribute.mAttributeIndex][i].a;
				SC_Memcpy(&aOutCreateParams.mVertexData[currentDataArrayPos], &vColor, sizeof(SC_Vector4));
				currentDataArrayPos += sizeof(SC_Vector4);
			}
			else if (vAttribute.mAttributeId == SR_VertexAttribute::BoneId)
			{
			}
			else if (vAttribute.mAttributeId == SR_VertexAttribute::BoneWeight)
			{
			}
		}
	}
}

void SED_AssimpMesh::ExtractIndices(SGfx_MeshCreateParams& aOutCreateParams) const
{
	const uint32 numIndices = mImportedMesh->mNumFaces * 3;
	const bool use16BitIndices = (numIndices < SC_UINT16_MAX);
	aOutCreateParams.mIndexStride = (use16BitIndices) ? sizeof(uint16) : sizeof(uint32);
	aOutCreateParams.mIndexData.Respace(numIndices * aOutCreateParams.mIndexStride);

	uint32 currentDataArrayPos = 0;
	for (uint32 faceIdx = 0; faceIdx < mImportedMesh->mNumFaces; ++faceIdx)
	{
		const aiFace& face = mImportedMesh->mFaces[faceIdx];
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

bool SED_AssimpMesh::GenerateMeshlets(SGfx_MeshCreateParams& aOutCreateParams) const
{
	const uint32 numVertices = aOutCreateParams.mVertexData.Count() / aOutCreateParams.mVertexLayout.GetVertexStrideSize();
	const uint32 numIndices = aOutCreateParams.mIndexData.Count() / aOutCreateParams.mIndexStride;

	bool result = SGfx_GenerateMeshlets(
		64,
		64,
		aOutCreateParams.mVertexData.GetBuffer(),
		aOutCreateParams.mVertexLayout,
		numVertices,
		aOutCreateParams.mIndexData.GetBuffer(),
		(aOutCreateParams.mIndexStride == sizeof(uint16)) ? true : false,
		numIndices,
		aOutCreateParams.mMeshlets,
		aOutCreateParams.mPrimitiveIndices,
		aOutCreateParams.mVertexIndices);

	aOutCreateParams.mVertexIndicesStride = aOutCreateParams.mIndexStride;
	return result;
}

SED_AssimpMesh::SED_AssimpMesh()
	: mImportedMesh(nullptr)
	, mMaterialIndex(SC_UINT32_MAX)
{
}

SED_AssimpMesh::~SED_AssimpMesh()
{

}

SC_Ref<SGfx_Mesh> SED_AssimpMesh::GetMesh() const
{
	return SGfx_Mesh::Create(mMeshParams);
}

bool SED_AssimpMesh::SaveToDisk(const SC_FilePath& aFileDirectory) const
{
	SC_FilePath gameDirPath(SC_EnginePaths::Get().GetGameDataDirectory());
	SC_FilePath::CreateDirectory(gameDirPath + aFileDirectory);

	SC_FilePath path(aFileDirectory);
	path += std::string("/");
	path += std::string(mImportedMesh->mName.C_Str());
	path.RemoveExtension();
	path += std::string(".smf");

	SC_FilePath fullPath(gameDirPath + path);
	bool result = true;
	if (!SC_FilePath::Exists(fullPath))
	{
		result = SGfx_MeshLoader::Save(fullPath, mMeshParams);
	}

	if (result)
		mMeshParams.mSourceFile = path;

	return result;
}

uint32 SED_AssimpMesh::GetMaterialIndex() const
{
	return mMaterialIndex;
}

const SC_FilePath& SED_AssimpMesh::GetSavePath() const
{
	return mMeshParams.mSourceFile;
}

SED_AssimpScene::SED_AssimpScene()
	: mImportedScene(nullptr)

{

}

SED_AssimpScene::~SED_AssimpScene()
{
	//delete mImportedScene;
	mImportedScene = nullptr;
}

bool SED_AssimpScene::Init(const SC_FilePath& aSourceFile)
{
	mSourceFile = aSourceFile;

	mMaterials.Respace(mImportedScene->mNumMaterials);
	for (uint32 materialIdx = 0; materialIdx < mImportedScene->mNumMaterials; ++materialIdx)
		mMaterials[materialIdx] = SC_Move(SED_AssimpMaterial(mImportedScene->mMaterials[materialIdx], mSourceFile));

	mMeshes.Respace(mImportedScene->mNumMeshes);
	for (uint32 meshIdx = 0; meshIdx < mImportedScene->mNumMeshes; ++meshIdx)
		mMeshes[meshIdx] = SC_Move(SED_AssimpMesh(mImportedScene->mMeshes[meshIdx], mImportedScene->mMeshes[meshIdx]->mMaterialIndex, mSourceFile));

	return true;
}

uint32 SED_AssimpScene::GetNumMeshes() const
{
	return mMeshes.Count();
}

uint32 SED_AssimpScene::GetNumMaterials() const
{
	return mMaterials.Count();
}

bool SED_AssimpScene::ConvertToLevelAndSave(SGF_Level& aOutLevel)
{
	if (!mImportedScene || !mImportedScene->mRootNode)
		return false;

	SC_FilePath saveDir("/ImportedMeshes");
	for (const SED_AssimpMesh& mesh : mMeshes)
		mesh.SaveToDisk(saveDir);

	VisitNode(mImportedScene->mRootNode, aOutLevel);
	return true;
}

void SED_AssimpScene::VisitNode(aiNode* aNode, SGF_Level& aOutLevel)
{
	aiVector3D aiPosition;
	aiVector3D aiRotation;
	aiVector3D aiScale;
	aNode->mTransformation.Decompose(aiScale, aiRotation, aiPosition);

	SC_Vector position = SC_Vector(aiPosition.x, aiPosition.y, aiPosition.z);
	SC_Vector scale = SC_Vector(aiScale.x, aiScale.y, aiScale.z);
	SC_Quaternion rotation = SC_Quaternion::FromEulerAngles(SC_Vector(aiRotation.x, aiRotation.y, aiRotation.z), false);

	for (uint32 i = 0; i < aNode->mNumMeshes; ++i)
	{
		uint32 meshIdx = aNode->mMeshes[i];
		SED_AssimpMesh& importedMesh = mMeshes[meshIdx];
		std::string nodeName = aNode->mName.C_Str();

		SGF_EntityManager* entityManager = aOutLevel.GetWorld()->GetEntityManager();
		SGF_Entity entity = entityManager->CreateEntity();
		entity.AddComponent<SGF_EntityIdComponent>();
		entity.AddComponent<SGF_EntityNameComponent>()->mName = nodeName;

		SGF_TransformComponent* transform = entity.AddComponent<SGF_TransformComponent>();
		transform->mPosition = position;
		transform->mRotation = rotation;
		transform->mScale = scale;

		SGF_StaticMeshComponent* meshComponent = entity.AddComponent<SGF_StaticMeshComponent>();
		meshComponent->SetMesh(SGfx_MeshInstance::Create(importedMesh.GetMesh()));
		meshComponent->SetMaterial(SGfx_MaterialInstance::Create(mMaterials[importedMesh.GetMaterialIndex()].GetMaterial()));

		aOutLevel.AddEntity(entity);
	}

	for (uint32 i = 0; i < aNode->mNumChildren; ++i)
		VisitNode(aNode->mChildren[i], aOutLevel);
}

SED_AssimpImporter::SED_AssimpImporter()
{
	mImporter = SC_MakeUnique<Assimp::Importer>();
}

SED_AssimpImporter::~SED_AssimpImporter()
{

}

bool SED_AssimpImporter::ImportScene(const SC_FilePath& aFilePath, SED_AssimpScene& aOutScene, float aImportScale)
{
	mImporter->SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, aImportScale);

	uint32 flags = aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded | aiProcess_TransformUVCoords | aiProcess_GlobalScale | aiProcess_CalcTangentSpace;
	const aiScene* importedScene = mImporter->ReadFile(aFilePath.GetAbsolutePath(), flags);

	if (!importedScene)
		return false;

	aOutScene.mImportedScene = mImporter->GetOrphanedScene();
	return aOutScene.Init(aFilePath);
}
