#pragma once
#include "SGfx_Mesh.h"

struct SGfx_MeshCreateParams;
class SC_FilePath;
class SGfx_ModelLoader
{
public:
	static constexpr const char* gFileExtension = ".smf";

	static bool Save(const SC_FilePath& aFilePath, const SC_Array<SGfx_MeshCreateParams>& aCreateParams);
	static bool Load(const SC_FilePath& aFilePath, SC_Array<SGfx_MeshCreateParams>& aOutCreateParams);

private:
	static constexpr uint32 gCurrentSMFVersion = 1;
	struct ModelTOC
	{
		uint32 mNumMeshes;
		uint32 mVersion;
	};
	struct MeshTOC
	{
		SC_Vector mAABBMin;
		SC_Vector mAABBMax;
		uint32 mNumVertices;
		uint32 mNumMeshlets;
		uint32 mMeshletStride;
		uint32 mNumPrimitiveIndices;
		uint32 mPrimitiveIndicesStride;
		uint32 mNumVertexIndices;
		uint32 mVertexIndicesStride;
		SR_VertexLayout mVertexLayout;
	};
};

