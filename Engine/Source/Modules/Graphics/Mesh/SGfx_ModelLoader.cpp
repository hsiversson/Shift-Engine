#include "SGfx_ModelLoader.h"
#include "SGfx_Mesh.h"

#include <fstream>

bool SGfx_ModelLoader::Save(const SC_FilePath& aFilePath, const SC_Array<SGfx_MeshCreateParams>& aCreateParams)
{
	std::ofstream outStream(aFilePath.GetAbsolutePath(), std::ofstream::out | std::ofstream::binary);

	if (outStream.is_open())
	{
		ModelTOC modelTOC;
		modelTOC.mVersion = gCurrentSMFVersion;
		modelTOC.mNumMeshes = aCreateParams.Count();

		outStream.write(reinterpret_cast<char*>(&modelTOC), sizeof(ModelTOC));

		uint32 paramIdx = 0;
		for (const SGfx_MeshCreateParams& createParam : aCreateParams)
		{
			const uint32 vertexStrideSize = aCreateParams[paramIdx].mVertexLayout.GetVertexStrideSize();

			MeshTOC meshTOC;
			meshTOC.mAABBMax = createParam.mAABBMax;
			meshTOC.mAABBMin = createParam.mAABBMin;
			meshTOC.mNumVertices = aCreateParams[paramIdx].mVertexData.Count() / vertexStrideSize;
			meshTOC.mNumMeshlets = aCreateParams[paramIdx].mMeshlets.Count();
			meshTOC.mMeshletStride = sizeof(SGfx_Meshlet);
			meshTOC.mNumPrimitiveIndices = aCreateParams[paramIdx].mPrimitiveIndices.Count();
			meshTOC.mPrimitiveIndicesStride = sizeof(SGfx_PackedPrimitiveTriangle);
			meshTOC.mVertexIndicesStride = aCreateParams[paramIdx].mVertexIndicesStride;
			meshTOC.mNumVertexIndices = aCreateParams[paramIdx].mVertexIndices.Count() / meshTOC.mVertexIndicesStride;
			meshTOC.mVertexLayout = aCreateParams[paramIdx].mVertexLayout;

			outStream.write(reinterpret_cast<const char*>(&meshTOC), sizeof(MeshTOC));
			outStream.write(reinterpret_cast<const char*>(aCreateParams[paramIdx].mVertexData.GetBuffer()), vertexStrideSize * meshTOC.mNumVertices);
			outStream.write(reinterpret_cast<const char*>(aCreateParams[paramIdx].mMeshlets.GetBuffer()), meshTOC.mMeshletStride * meshTOC.mNumMeshlets);
			outStream.write(reinterpret_cast<const char*>(aCreateParams[paramIdx].mPrimitiveIndices.GetBuffer()), meshTOC.mPrimitiveIndicesStride * meshTOC.mNumPrimitiveIndices);
			outStream.write(reinterpret_cast<const char*>(aCreateParams[paramIdx].mVertexIndices.GetBuffer()), meshTOC.mVertexIndicesStride * meshTOC.mNumVertexIndices);
			++paramIdx;
		}
	}
	else
		return false;

	return true;
}

bool SGfx_ModelLoader::Load(const SC_FilePath& aFilePath, SC_Array<SGfx_MeshCreateParams>& aOutCreateParams)
{
	std::ifstream inStream(aFilePath.GetAbsolutePath(), std::ifstream::in | std::ifstream::binary);
	if (inStream.is_open())
	{
		ModelTOC modelTOC;
		inStream.read(reinterpret_cast<char*>(&modelTOC), sizeof(ModelTOC));

		aOutCreateParams.Reserve(modelTOC.mNumMeshes);
		for (uint32 i = 0; i < modelTOC.mNumMeshes; ++i)
		{
			SGfx_MeshCreateParams& createParams = aOutCreateParams.Add();

			MeshTOC meshTOC;
			inStream.read(reinterpret_cast<char*>(&meshTOC), sizeof(MeshTOC));

			createParams.mAABBMin = meshTOC.mAABBMin;
			createParams.mAABBMax = meshTOC.mAABBMax;
			createParams.mVertexLayout = meshTOC.mVertexLayout;
			createParams.mVertexIndicesStride = meshTOC.mVertexIndicesStride;

			const uint32 vertexDataSize = meshTOC.mNumVertices * meshTOC.mVertexLayout.GetVertexStrideSize();
			const uint32 meshletDataSize = meshTOC.mNumMeshlets * meshTOC.mMeshletStride;
			const uint32 primitiveIndicesDataSize = meshTOC.mNumPrimitiveIndices * meshTOC.mPrimitiveIndicesStride;
			const uint32 vertexIndicesDataSize = meshTOC.mNumVertexIndices * meshTOC.mVertexIndicesStride;

			createParams.mVertexData.Respace(vertexDataSize);
			createParams.mMeshlets.Respace(meshTOC.mNumMeshlets);
			createParams.mPrimitiveIndices.Respace(meshTOC.mNumPrimitiveIndices);
			createParams.mVertexIndices.Respace(vertexIndicesDataSize);

			inStream.read(reinterpret_cast<char*>(createParams.mVertexData.GetBuffer()), vertexDataSize);
			inStream.read(reinterpret_cast<char*>(createParams.mMeshlets.GetBuffer()), meshletDataSize);
			inStream.read(reinterpret_cast<char*>(createParams.mPrimitiveIndices.GetBuffer()), primitiveIndicesDataSize);
			inStream.read(reinterpret_cast<char*>(createParams.mVertexIndices.GetBuffer()), vertexIndicesDataSize);
		}
	}
	else
		return false;

	return true;
}
