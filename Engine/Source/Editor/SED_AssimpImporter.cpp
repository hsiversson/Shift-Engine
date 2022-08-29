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

void GetMTLTextureMapping(SC_Array<uint32>& aOutTextureMappings)
{
	aOutTextureMappings.Add(aiTextureType_DIFFUSE); // BaseColor (map_Kd)
	aOutTextureMappings.Add(aiTextureType_HEIGHT);	// Normal (map_bump)
	aOutTextureMappings.Add(aiTextureType_SHININESS);// Roughness (map_Ns)
	aOutTextureMappings.Add(aiTextureType_AMBIENT); // Metallic (map_Ka)
	aOutTextureMappings.Add(aiTextureType_SPECULAR); // Metallic (map_Ka)
	aOutTextureMappings.Add(aiTextureType_EMISSIVE); // Emissive
	//aOutTextureMappings.Add(aiTextureType_OPACITY); // Opacity (Mask) (map_d)
}

void GetFBXTextureMapping(SC_Array<uint32>& aOutTextureMappings)
{
	aOutTextureMappings.Add(aiTextureType_DIFFUSE); // BaseColor (map_Kd)
	aOutTextureMappings.Add(aiTextureType_HEIGHT);	// Normal (map_bump)
	aOutTextureMappings.Add(aiTextureType_SHININESS);// Roughness (map_Ns)
	aOutTextureMappings.Add(aiTextureType_AMBIENT); // Metallic (map_Ka)
	aOutTextureMappings.Add(aiTextureType_SPECULAR); // Metallic (map_Ka)
	aOutTextureMappings.Add(aiTextureType_EMISSIVE); // Emissive
	//aOutTextureMappings.Add(aiTextureType_OPACITY); // Opacity (Mask) (map_d)
}

SED_AssimpMaterial::SED_AssimpMaterial(aiMaterial* aMaterial, const SC_FilePath& aSourceFile)
	: mImportedMaterial(aMaterial)
	, mSourceDir(aSourceFile.GetParentDirectory())
{
	bool useAlphaTesting = false;

	SC_Array<uint32> validTextureTypes;
	GetMTLTextureMapping(validTextureTypes);
	//const uint32 validTextureTypes[] = {
	//	//aiTextureType_BASE_COLOR,
	//	aiTextureType_DIFFUSE,
	//
	//	//aiTextureType_NORMALS,
	//	aiTextureType_HEIGHT, // FOR OBJ-MTL FILES
	//	//aiTextureType_NORMAL_CAMERA,
	//	// 
	//	//aiTextureType_DIFFUSE_ROUGHNESS,
	//	aiTextureType_SPECULAR, // FOR OBJ-MTL FILES
	//
	//	//aiTextureType_METALNESS,
	//	aiTextureType_AMBIENT, // FOR OBJ-MTL FILES
	//
	//	aiTextureType_AMBIENT_OCCLUSION,
	//
	//	//aiTextureType_SPECULAR,
	//
	//	aiTextureType_OPACITY,
	//	aiTextureType_EMISSIVE,
	//	//aiTextureType_EMISSION_COLOR,
	//};

	aiString path;
	for (uint32 i = 0; i < validTextureTypes.Count(); ++i)
	{
		const uint32 currentTextureType = validTextureTypes[i];
		const aiTextureType texType = (aiTextureType)currentTextureType;

		aiReturn texFound = mImportedMaterial->GetTexture(texType, 0, &path);
		SC_FilePath texturePath(path.data);
		SC_FilePath truePath(mSourceDir + "/" + texturePath);

		bool found = (texFound == AI_SUCCESS) && SC_FilePath::Exists(truePath);
		if (found)
		{
			if (texType == aiTextureType_OPACITY)
				useAlphaTesting = true;

			mMaterialProperties.mTextures.Add(truePath);
		}
		else
		{
			switch (texType)
			{
			case aiTextureType_BASE_COLOR:
			case aiTextureType_DIFFUSE:
			case aiTextureType_DIFFUSE_ROUGHNESS:
			case aiTextureType_SHININESS:
				mMaterialProperties.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Grey_1x1.dds");
				break;
			case aiTextureType_NORMALS:
			case aiTextureType_NORMAL_CAMERA:
			case aiTextureType_HEIGHT:
				mMaterialProperties.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Normal_1x1.dds");
				break;
			case aiTextureType_METALNESS:
			case aiTextureType_AMBIENT:
				mMaterialProperties.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Black_1x1.dds");
				break;
			case aiTextureType_EMISSIVE:
			case aiTextureType_EMISSION_COLOR:
				mMaterialProperties.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_Black_1x1.dds");
				break;
			case aiTextureType_AMBIENT_OCCLUSION:
			case aiTextureType_SPECULAR:
			case aiTextureType_OPACITY:
				mMaterialProperties.mTextures.Add(SC_EnginePaths::Get().GetEngineDataDirectory() + "/Textures/Default_White_1x1.dds");
				break;
			}
		}
	}

	int twoSided = 0;
	mImportedMaterial->Get(AI_MATKEY_TWOSIDED, twoSided);


	mMaterialProperties.mShaderProperties.mRasterizerProperties.mCullMode = (twoSided) ? SR_CullMode::None : SR_CullMode::Back;
	//mMaterialProperties.mShaderProperties.mRasterizerProperties.mWireframe = true;
	mMaterialProperties.mShaderProperties.mBlendStateProperties.mNumRenderTargets = 1;
	mMaterialProperties.mShaderProperties.mRTVFormats.mNumColorFormats = 1;
	mMaterialProperties.mShaderProperties.mRTVFormats.mColorFormats[0] = SR_Format::RG11B10_FLOAT;
	mMaterialProperties.mShaderProperties.mDepthStencilProperties.mWriteDepth = false;
	mMaterialProperties.mShaderProperties.mDepthStencilProperties.mDepthComparisonFunc = SR_ComparisonFunc::Equal;

	SR_DirectXShaderCompiler compiler;
	SR_ShaderCompileArgs args;
	args.mEntryPoint = "Main";
	args.mShaderFile = SC_EnginePaths::Get().GetEngineDataDirectory() + "/Shaders/PixelShaderDefault.ssf";

	//args.mDefines.Add(SC_Pair<std::string, std::string>("USE_PACKED_NORMALMAP", "1"));

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

SED_AssimpMesh::SED_AssimpMesh(aiMesh* aMesh, uint32 aMeshIndex, uint32 aMaterialIndex, const SC_FilePath& aSourceFile)
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

	//if (mImportedMesh->HasBones())
	//{
	//	for (uint32 boneIndex = 0; boneIndex < mImportedMesh->mNumBones; ++boneIndex)
	//	{
	//		SR_VertexAttribute boneAttribute = static_cast<SR_VertexAttribute>(static_cast<uint32>(SR_VertexAttribute::BoneId) + boneIndex);
	//		SR_VertexAttribute boneWeightAttribute = static_cast<SR_VertexAttribute>(static_cast<uint32>(SR_VertexAttribute::BoneWeight) + boneIndex);
	//		vertexLayout.SetAttribute(boneAttribute, SR_Format::R32_UINT, boneIndex);
	//		vertexLayout.SetAttribute(boneWeightAttribute, SR_Format::R32_FLOAT, boneIndex);
	//	}
	//}

	ExtractVertices(mMeshParams);
	ExtractIndices(mMeshParams);
	GenerateMeshlets(mMeshParams);
	mMeshParams.mIsMeshletData = true;
	mName = SC_FormatStr("{}_{}", mImportedMesh->mName.C_Str(), std::to_string(aMeshIndex).c_str());
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
				SC_Vector2 vUV;
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
			//else if (vAttribute.mAttributeId == SR_VertexAttribute::BoneId)
			//{
			//}
			//else if (vAttribute.mAttributeId == SR_VertexAttribute::BoneWeight)
			//{
			//}
		}
	}
}

void SED_AssimpMesh::ExtractIndices(SGfx_MeshCreateParams& aOutCreateParams) const
{
	static constexpr uint32 gMaxNumFaceIndices = 3; // For triangle

	const uint32 numIndices = mImportedMesh->mNumFaces * gMaxNumFaceIndices;
	const bool use16BitIndices = numIndices < SC_UINT16_MAX;
	aOutCreateParams.mIndexStride = (use16BitIndices) ? sizeof(uint16) : sizeof(uint32);
	aOutCreateParams.mIndexData.Respace(numIndices * aOutCreateParams.mIndexStride);

	const uint32 faceStride = aOutCreateParams.mIndexStride * gMaxNumFaceIndices;

	uint32 currentDataArrayPos = 0;
	for (uint32 faceIdx = 0; faceIdx < mImportedMesh->mNumFaces; ++faceIdx)
	{
		const aiFace& face = mImportedMesh->mFaces[faceIdx];
		assert(face.mNumIndices == gMaxNumFaceIndices && "Mesh isn't triangulated.");

		if (use16BitIndices)
		{
			uint16 indices[gMaxNumFaceIndices] = { (uint16)face.mIndices[0], (uint16)face.mIndices[1], (uint16)face.mIndices[2] };
			SC_Memcpy(&aOutCreateParams.mIndexData[currentDataArrayPos], &indices, faceStride);
			currentDataArrayPos += faceStride;
		}
		else
		{
			uint32 indices[gMaxNumFaceIndices] = { (uint32)face.mIndices[0], (uint32)face.mIndices[1], (uint32)face.mIndices[2] };
			SC_Memcpy(&aOutCreateParams.mIndexData[currentDataArrayPos], &indices, faceStride);
			currentDataArrayPos += faceStride;
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
	path += mName;
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
	{
		aiMesh* importedMesh = mImportedScene->mMeshes[meshIdx];
		mMeshes[meshIdx] = SC_Move(SED_AssimpMesh(importedMesh, meshIdx, importedMesh->mMaterialIndex, mSourceFile));
	}

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

static void VisitNode(aiNode* aNode, SGF_Level& aOutLevel, const aiMatrix4x4& aParentTransform, const SC_Array<SED_AssimpMesh>& aMeshes, const SC_Array<SED_AssimpMaterial>& aMaterials)
{
	aiMatrix4x4 nodeWorldTransform = aParentTransform * aNode->mTransformation;
	if (aNode->mNumMeshes > 0)
	{
		aiVector3D aiPosition;
		aiVector3D aiRotation;
		aiVector3D aiScale;
		nodeWorldTransform.Decompose(aiScale, aiRotation, aiPosition);

		SC_Vector position = SC_Vector(aiPosition.x, aiPosition.y, aiPosition.z);
		SC_Vector scale = SC_Vector(aiScale.x, aiScale.y, aiScale.z);
		SC_Quaternion rotation = SC_Quaternion::FromEulerAngles(SC_Vector(aiRotation.x, aiRotation.y, aiRotation.z), false);

		const std::string nodeName = aNode->mName.C_Str();
		for (uint32 i = 0; i < aNode->mNumMeshes; ++i)
		{
			const SED_AssimpMesh& importedMesh = aMeshes[aNode->mMeshes[i]];
			const SED_AssimpMaterial& importedMaterial = aMaterials[importedMesh.GetMaterialIndex()];

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
			meshComponent->SetMaterial(SGfx_MaterialInstance::Create(importedMaterial.GetMaterial()));

			aOutLevel.AddEntity(entity);
		}
	}
	for (uint32 i = 0; i < aNode->mNumChildren; ++i)
		VisitNode(aNode->mChildren[i], aOutLevel, nodeWorldTransform, aMeshes, aMaterials);
}

bool SED_AssimpScene::ConvertToLevelAndSave(SGF_Level& aOutLevel)
{
	if (!mImportedScene || !mImportedScene->mRootNode)
		return false;

	SC_FilePath saveDir("/ImportedMeshes");
	for (const SED_AssimpMesh& mesh : mMeshes)
		mesh.SaveToDisk(saveDir);

	VisitNode(mImportedScene->mRootNode, aOutLevel, mImportedScene->mRootNode->mTransformation, mMeshes, mMaterials);
	return true;
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

	uint32 flags = aiProcess_GlobalScale | aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded;
	const aiScene* importedScene = mImporter->ReadFile(aFilePath.GetAbsolutePath(), flags);

	if (!importedScene)
		return false;

	aOutScene.mImportedScene = mImporter->GetOrphanedScene();
	return aOutScene.Init(aFilePath);
}
