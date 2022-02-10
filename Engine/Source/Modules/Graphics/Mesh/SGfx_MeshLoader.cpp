#include "SGfx_MeshLoader.h"
#include "SGfx_Mesh.h"

#include <fstream>

bool SGfx_MeshLoader::Save(const SC_FilePath& aFilePath, const SGfx_MeshCreateParams& aCreateParams)
{
	std::ofstream outStream(aFilePath.GetAbsolutePath(), std::ofstream::out | std::ofstream::binary);

	if (outStream.is_open())
	{
		FileTOC fileTOC;
		fileTOC.mVersion = gCurrentSMFVersion;
		fileTOC.mHasMeshletData = aCreateParams.mIsMeshletData;

		outStream.write(reinterpret_cast<char*>(&fileTOC), sizeof(FileTOC));

		const uint32 vertexStrideSize = aCreateParams.mVertexLayout.GetVertexStrideSize();

		MeshTOC meshTOC;
		meshTOC.mAABBMax = aCreateParams.mAABBMax;
		meshTOC.mAABBMin = aCreateParams.mAABBMin;
		meshTOC.mNumVertices = aCreateParams.mVertexData.Count() / vertexStrideSize;
		meshTOC.mNumMeshlets = aCreateParams.mMeshlets.Count();
		meshTOC.mMeshletStride = sizeof(SGfx_Meshlet);
		meshTOC.mNumPrimitiveIndices = aCreateParams.mPrimitiveIndices.Count();
		meshTOC.mPrimitiveIndicesStride = sizeof(SGfx_PackedPrimitiveTriangle);
		meshTOC.mVertexIndicesStride = aCreateParams.mVertexIndicesStride;
		meshTOC.mNumVertexIndices = aCreateParams.mVertexIndices.Count() / meshTOC.mVertexIndicesStride;
		meshTOC.mVertexLayout = aCreateParams.mVertexLayout;

		outStream.write(reinterpret_cast<const char*>(&meshTOC), sizeof(MeshTOC));
		outStream.write(reinterpret_cast<const char*>(aCreateParams.mVertexData.GetBuffer()), vertexStrideSize * meshTOC.mNumVertices);
		outStream.write(reinterpret_cast<const char*>(aCreateParams.mMeshlets.GetBuffer()), meshTOC.mMeshletStride * meshTOC.mNumMeshlets);
		outStream.write(reinterpret_cast<const char*>(aCreateParams.mPrimitiveIndices.GetBuffer()), meshTOC.mPrimitiveIndicesStride * meshTOC.mNumPrimitiveIndices);
		outStream.write(reinterpret_cast<const char*>(aCreateParams.mVertexIndices.GetBuffer()), meshTOC.mVertexIndicesStride * meshTOC.mNumVertexIndices);
		
	}
	else
		return false;

	return true;
}

bool SGfx_MeshLoader::Load(const SC_FilePath& aFilePath, SGfx_MeshCreateParams& aOutCreateParams)
{
	std::ifstream inStream(aFilePath.GetAbsolutePath(), std::ifstream::in | std::ifstream::binary);
	if (inStream.is_open())
	{
		FileTOC fileTOC;
		inStream.read(reinterpret_cast<char*>(&fileTOC), sizeof(FileTOC));

		MeshTOC meshTOC;
		inStream.read(reinterpret_cast<char*>(&meshTOC), sizeof(MeshTOC));

		aOutCreateParams.mIsMeshletData = fileTOC.mHasMeshletData;
		aOutCreateParams.mAABBMin = meshTOC.mAABBMin;
		aOutCreateParams.mAABBMax = meshTOC.mAABBMax;
		aOutCreateParams.mVertexLayout = meshTOC.mVertexLayout;
		aOutCreateParams.mVertexIndicesStride = meshTOC.mVertexIndicesStride;

		const uint32 vertexDataSize = meshTOC.mNumVertices * meshTOC.mVertexLayout.GetVertexStrideSize();
		const uint32 meshletDataSize = meshTOC.mNumMeshlets * meshTOC.mMeshletStride;
		const uint32 primitiveIndicesDataSize = meshTOC.mNumPrimitiveIndices * meshTOC.mPrimitiveIndicesStride;
		const uint32 vertexIndicesDataSize = meshTOC.mNumVertexIndices * meshTOC.mVertexIndicesStride;

		aOutCreateParams.mVertexData.Respace(vertexDataSize);
		aOutCreateParams.mMeshlets.Respace(meshTOC.mNumMeshlets);
		aOutCreateParams.mPrimitiveIndices.Respace(meshTOC.mNumPrimitiveIndices);
		aOutCreateParams.mVertexIndices.Respace(vertexIndicesDataSize);

		inStream.read(reinterpret_cast<char*>(aOutCreateParams.mVertexData.GetBuffer()), vertexDataSize);
		inStream.read(reinterpret_cast<char*>(aOutCreateParams.mMeshlets.GetBuffer()), meshletDataSize);
		inStream.read(reinterpret_cast<char*>(aOutCreateParams.mPrimitiveIndices.GetBuffer()), primitiveIndicesDataSize);
		inStream.read(reinterpret_cast<char*>(aOutCreateParams.mVertexIndices.GetBuffer()), vertexIndicesDataSize);
	}
	else
		return false;

	aOutCreateParams.mSourceFile = aFilePath;
	return true;
}