#include "SGfx_MeshletGenerator.h"
#include <unordered_set>
#include <algorithm>

struct PackedTriangle
{
	PackedTriangle()
		: mIndex0(0)
		, mIndex1(0)
		, mIndex2(0)
		, __pad(0)
	{}
	PackedTriangle(const PackedTriangle& aOther)
		: mIndex0(aOther.mIndex0)
		, mIndex1(aOther.mIndex1)
		, mIndex2(aOther.mIndex2)
		, __pad(aOther.__pad)
	{}

	uint32 mIndex0 : 10;
	uint32 mIndex1 : 10;
	uint32 mIndex2 : 10;
	uint32 __pad : 2;
};

template<class IndexType>
struct InlineMeshlet
{
	SC_Array<IndexType> mUniqueVertexIndices;
	SC_Array<PackedTriangle> mPrimitiveIndices;
};

template<class IndexType>
static void BuildMeshlets(
	uint32 aMaxVertices,
	uint32 aMaxPrimitives,
	const uint8* aVertexData,
	const uint32 aNumVertices,
	const SR_VertexLayout& aVertexLayout,
	const IndexType* aIndexData,
	const uint32 aNumIndices,
	SC_Array<InlineMeshlet<IndexType>>& aOutMeshlets);

template<class IndexType>
static void BuildAdjacencyList(
	const IndexType* aIndexData,
	const uint32 aNumIndices,
	const uint8* aVertexData, 
	const uint32 aNumVertices,
	const SR_VertexLayout& aVertexLayout,
	SC_Array<uint32>& aOutAdjecencyList)
{
	const uint32 faceCount = aNumIndices / 3;
	const uint32 vertexStride = aVertexLayout.GetVertexStrideSize();
	const uint32 vertCount = aNumVertices;
	const uint8* vertexDataPointer = aVertexData;
	const uint32 hashSize = vertCount / 3;

	// Build positions
	SC_Array<uint32> points;
	points.Respace(vertCount);
	{
		std::unique_ptr<uint32[]> temp(new (std::nothrow) uint32[vertCount + aNumIndices]);
		SC_ASSERT(temp);

		uint32_t* vertexToCorner = temp.get();
		uint32_t* vertexCornerList = temp.get() + vertCount;

		SC_Memset(vertexToCorner, 0xff, sizeof(uint32) * vertCount);
		SC_Memset(vertexCornerList, 0xff, sizeof(uint32) * aNumIndices);

		for (uint32 j = 0; j < aNumIndices; ++j)
		{
			IndexType k = aIndexData[j];
			if (k == IndexType(-1))
				continue;

			if (k >= vertCount)
			{
				SC_ASSERT(false);
				return;
			}

			vertexCornerList[j] = vertexToCorner[k];
			vertexToCorner[k] = j;
		}

		struct VertexHashEntry
		{
			VertexHashEntry() : mIndex(SC_UINT32_MAX), mNext(nullptr) {}
			SC_Vector mVector;
			uint32 mIndex;
			VertexHashEntry* mNext;
		};
		std::unique_ptr<VertexHashEntry*[]> hashTableV(new VertexHashEntry*[hashSize]);
		std::unique_ptr<VertexHashEntry[]> entriesV(new VertexHashEntry[vertCount]);
		SC_Memset(hashTableV.get(), 0, sizeof(VertexHashEntry*) * hashSize);

		uint32 freeEntry = 0;
		for (uint32 vert = 0; vert < vertCount; ++vert)
		{
			const SC_Vector p = *((SC_Vector*)(vertexDataPointer + (vert * vertexStride)));
			const uint32* px = reinterpret_cast<const uint32*>(&p.x);
			const uint32* py = reinterpret_cast<const uint32*>(&p.y);
			const uint32* pz = reinterpret_cast<const uint32*>(&p.z);
			uint32 hashKey = (*px + *py + *pz) % uint32(hashSize);

			uint32 found = SC_UINT32_MAX;
			for (VertexHashEntry* current = hashTableV[hashKey]; current != nullptr; current = current->mNext)
			{
				if (current->mVector.x == p.x && current->mVector.y == p.y && current->mVector.z == p.z)
				{
					uint32 head = vertexToCorner[vert];

					bool isPresent = false;
					while (head != SC_UINT32_MAX)
					{
						uint32 face = head / 3;
						SC_ASSERT(face < faceCount);
						SC_ASSERT((aIndexData[face * 3] == vert) || (aIndexData[face * 3 + 1] == vert) || (aIndexData[face * 3 + 2] == vert));

						if ((aIndexData[face * 3] == current->mIndex) || (aIndexData[face * 3 + 1] == current->mIndex) || (aIndexData[face * 3 + 2] == current->mIndex))
						{
							isPresent = true;
							break;
						}

						head = vertexCornerList[head];
					}

					if (!isPresent)
					{
						found = current->mIndex;
						break;
					}
				}

				if (found != SC_UINT32_MAX)
				{
					points[vert] = found;
				}
				else
				{
					SC_ASSERT(freeEntry < vertCount);

					auto newEntry = &entriesV[freeEntry];
					++freeEntry;

					newEntry->mVector = p;
					newEntry->mIndex = vert;
					newEntry->mNext = hashTableV[hashKey];
					hashTableV[hashKey] = newEntry;

					points[vert] = vert;
				}
			}
		}
	}

	// Build Adjacencies
	{
		struct EdgeEntry
		{
			EdgeEntry() : mIndex0(SC_UINT32_MAX), mIndex1(SC_UINT32_MAX), mIndex2(SC_UINT32_MAX), mFace(SC_UINT32_MAX), mNext(nullptr) {}

			uint32   mIndex0;
			uint32   mIndex1;
			uint32   mIndex2;
			uint32   mFace;
			EdgeEntry* mNext;
		};
		std::unique_ptr<EdgeEntry*[]> hashTable(new EdgeEntry*[hashSize]);
		std::unique_ptr<EdgeEntry[]> entries(new EdgeEntry[faceCount * 3]);
		SC_Memset(hashTable.get(), 0, sizeof(EdgeEntry*) * hashSize);

		uint32 freeEntry = 0;
		for (uint32 face = 0; face < faceCount; ++face)
		{
			IndexType i0 = aIndexData[face * 3];
			IndexType i1 = aIndexData[face * 3 + 1];
			IndexType i2 = aIndexData[face * 3 + 2];

			if (i0 == IndexType(-1) || i1 == IndexType(-1) || i2 == IndexType(-1))
				continue;

			uint32 v1 = points[i0];
			uint32 v2 = points[i1];
			uint32 v3 = points[i2];

			if (v1 == v2 || v1 == v3 || v2 == v3)
				continue;

			for (uint32 p = 0; p < 3; ++p)
			{
				uint32 va = points[aIndexData[face * 3 + p]];
				uint32 vb = points[aIndexData[face * 3 + ((p + 1) % 3)]];
				uint32 vc = points[aIndexData[face * 3 + ((p + 2) % 3)]];

				uint32 hashKey = va % hashSize;

				auto entry = &entries[freeEntry];
				++freeEntry;

				entry->mIndex0 = va;
				entry->mIndex1 = vb;
				entry->mIndex2 = vc;
				entry->mFace = face;
				entry->mNext = hashTable[hashKey];
				hashTable[hashKey] = entry;
			}
		}

		SC_Memset(aOutAdjecencyList.GetBuffer(), SC_UINT32_MAX, aNumIndices * sizeof(uint32));

		for (uint32 face = 0; face < faceCount; ++face)
		{
			IndexType i0 = aIndexData[face * 3];
			IndexType i1 = aIndexData[face * 3 + 1];
			IndexType i2 = aIndexData[face * 3 + 2];

			if (i0 == IndexType(-1) || i1 == IndexType(-1) || i2 == IndexType(-1))
				continue;

			SC_ASSERT(i0 < vertCount);
			SC_ASSERT(i1 < vertCount);
			SC_ASSERT(i2 < vertCount);

			uint32 v1 = points[i0];
			uint32 v2 = points[i1];
			uint32 v3 = points[i2];

			if (v1 == v2 || v1 == v3 || v2 == v3)
				continue;

			for (uint32 point = 0; point < 3; ++point)
			{
				if (aOutAdjecencyList[face * 3 + point] != SC_UINT32_MAX)
					continue;

				// Look for edges directed in the opposite direction.
				uint32 va = points[aIndexData[face * 3 + ((point + 1) % 3)]];
				uint32 vb = points[aIndexData[face * 3 + point]];
				uint32 vc = points[aIndexData[face * 3 + ((point + 2) % 3)]];

				// Find a face sharing this edge
				uint32 hashKey = va % hashSize;

				EdgeEntry* current = hashTable[hashKey];
				EdgeEntry* previous = nullptr;
				uint32 foundFace = SC_UINT32_MAX;

				while (current != nullptr)
				{
					if ((current->mIndex1 == vb) && (current->mIndex0 == va))
					{
						foundFace = current->mFace;
						break;
					}

					previous = current;
					current = current->mNext;
				}

				EdgeEntry* found = current;
				EdgeEntry* foundPrev = previous;

				float bestDiff = -2.f;
				if (current)
				{
					previous = current;
					current = current->mNext;

					while (current != nullptr)
					{
						if ((current->mIndex1 == vb) && (current->mIndex0 == va))
						{
							SC_Vector pB1 = *((SC_Vector*)(vertexDataPointer + (vb * vertexStride)));
							SC_Vector pB2 = *((SC_Vector*)(vertexDataPointer + (va * vertexStride)));
							SC_Vector pB3 = *((SC_Vector*)(vertexDataPointer + (vc * vertexStride)));

							SC_Vector v12 = pB1 - pB2;
							SC_Vector v13 = pB1 - pB3;

							SC_Vector bNormal = v12.Cross(v13).GetNormalized();

							if (bestDiff == -2.f)
							{
								SC_Vector pA1 = *((SC_Vector*)(vertexDataPointer + (found->mIndex0 * vertexStride)));
								SC_Vector pA2 = *((SC_Vector*)(vertexDataPointer + (found->mIndex1 * vertexStride)));
								SC_Vector pA3 = *((SC_Vector*)(vertexDataPointer + (found->mIndex2 * vertexStride)));

								v12 = pA1 - pA2;
								v13 = pA1 - pA3;

								SC_Vector aNormal = v12.Cross(v13).GetNormalized();
								bestDiff = aNormal.Dot(bNormal);
							}

							SC_Vector pA1 = *((SC_Vector*)(vertexDataPointer + (current->mIndex0 * vertexStride)));
							SC_Vector pA2 = *((SC_Vector*)(vertexDataPointer + (current->mIndex1 * vertexStride)));
							SC_Vector pA3 = *((SC_Vector*)(vertexDataPointer + (current->mIndex2 * vertexStride)));

							v12 = pA1 - pA2;
							v13 = pA1 - pA3;

							SC_Vector aNormal = v12.Cross(v13).GetNormalized();
							float diff = aNormal.Dot(bNormal);

							// if face normals are closer, use new match
							if (diff > bestDiff)
							{
								found = current;
								foundPrev = previous;
								foundFace = current->mFace;
								bestDiff = diff;
							}
						}

						previous = current;
						current = current->mNext;
					}
				}

				// Update hash table and adjacency list
				if (foundFace != SC_UINT32_MAX)
				{
					// Erase the found from the hash table linked list.
					if (foundPrev != nullptr)
						foundPrev->mNext = found->mNext;
					else
						hashTable[hashKey] = found->mNext;

					aOutAdjecencyList[face * 3 + point] = foundFace;

					uint32 hashKey2 = vb % hashSize;
					current = hashTable[hashKey2];
					previous = nullptr;
					while (current != nullptr)
					{
						if ((current->mFace == face) && (current->mIndex1 == va) && (current->mIndex0 == vb))
						{
							if (previous != nullptr)
								previous->mNext = current->mNext;
							else
								hashTable[hashKey2] = current->mNext;

							break;
						}

						previous = current;
						current = current->mNext;
					}

					bool linked = false;
					for (uint32 point2 = 0; point2 < point; ++point2)
					{
						if (found->mFace == aOutAdjecencyList[face * 3 + point2])
						{
							linked = true;
							aOutAdjecencyList[face * 3 + point] = SC_UINT32_MAX;
							break;
						}
					}

					if (!linked)
					{
						uint32 point2 = 0;
						for (; point2 < 3; ++point2)
						{
							IndexType k = aIndexData[foundFace * 3 + point2];
							if (k == SC_UINT32_MAX)
								continue;

							if (points[k] == va)
								break;
						}

						if (point2 < 3)
							aOutAdjecencyList[found->mFace * 3 + point2] = face;
					}
				}
			}
		}
	}
}

template<class IndexType>
static bool Generate(
	uint32 aMaxVertices,
	uint32 aMaxPrimitives,
	const uint8* aVertexData,
	const SR_VertexLayout& aVertexLayout,
	const uint32 aNumVertices,
	const IndexType* aIndexData,
	const uint32 aNumIndices,
	SC_Array<SGfx_Meshlet>& aOutMeshlets,
	SC_Array<SGfx_PackedPrimitiveTriangle>& aOutPrimitives,
	SC_Array<uint8>& aOutVertexIndices)
{
	SC_Array<InlineMeshlet<IndexType>> generatedMeshlets;
	BuildMeshlets(aMaxVertices, aMaxPrimitives, aVertexData, aNumVertices, aVertexLayout, aIndexData, aNumIndices, generatedMeshlets);

	uint32 primitiveIndexCount = 0;
	uint32 uniqueVertexIndexCount = 0;

	uint32 meshletCount = aOutMeshlets.Count();
	aOutMeshlets.Respace(generatedMeshlets.Count());
	for (uint32 i = 0, dest = meshletCount; i < generatedMeshlets.Count(); ++i, ++dest)
	{
		aOutMeshlets[dest].mPrimitiveOffset = primitiveIndexCount;
		aOutMeshlets[dest].mPrimitiveCount = generatedMeshlets[i].mPrimitiveIndices.Count();
		primitiveIndexCount += generatedMeshlets[i].mPrimitiveIndices.Count();

		aOutMeshlets[dest].mVertexOffset = uniqueVertexIndexCount;
		aOutMeshlets[dest].mVertexCount = generatedMeshlets[i].mUniqueVertexIndices.Count();
		uniqueVertexIndexCount += generatedMeshlets[i].mUniqueVertexIndices.Count();
	}

	aOutPrimitives.Respace(primitiveIndexCount);
	aOutVertexIndices.Respace(uniqueVertexIndexCount * sizeof(IndexType));

	uint32* primDest = reinterpret_cast<uint32*>(aOutPrimitives.GetBuffer());
	IndexType* vertDest = reinterpret_cast<IndexType*>(aOutVertexIndices.GetBuffer());

	for (uint32 i = 0; i < generatedMeshlets.Count(); ++i)
	{
		SC_Memcpy(vertDest, generatedMeshlets[i].mUniqueVertexIndices.GetBuffer(), generatedMeshlets[i].mUniqueVertexIndices.Count() * sizeof(IndexType));
		SC_Memcpy(primDest, generatedMeshlets[i].mPrimitiveIndices.GetBuffer(), generatedMeshlets[i].mPrimitiveIndices.Count() * sizeof(uint32));

		vertDest += generatedMeshlets[i].mUniqueVertexIndices.Count();
		primDest += generatedMeshlets[i].mPrimitiveIndices.Count();
	}
	return true;
}

bool SGfx_GenerateMeshlets(
	uint32 aMaxVertices,
	uint32 aMaxPrimitives,
	const uint8* aVertexData,
	const SR_VertexLayout& aVertexLayout,
	const uint32 aNumVertices,
	const uint8* aIndexData,
	bool aUse16BitIndices,
	const uint32 aNumIndices,
	SC_Array<SGfx_Meshlet>& aOutMeshlets,
	SC_Array<SGfx_PackedPrimitiveTriangle>& aOutPrimitives,
	SC_Array<uint8>& aOutVertexIndices)
{
	if (aUse16BitIndices)
	{
		const uint16* indexData = reinterpret_cast<const uint16*>(aIndexData);
		return Generate(aMaxVertices, aMaxPrimitives, aVertexData, aVertexLayout, aNumVertices, indexData, aNumIndices, aOutMeshlets, aOutPrimitives, aOutVertexIndices);
	}
	else
	{
		const uint32* indexData = reinterpret_cast<const uint32*>(aIndexData);
		return Generate(aMaxVertices, aMaxPrimitives, aVertexData, aVertexLayout, aNumVertices, indexData, aNumIndices, aOutMeshlets, aOutPrimitives, aOutVertexIndices);
	}
}

bool SGfx_FlattenMeshletPrimitivesToIndexBuffer(
	const SC_Array<SGfx_Meshlet>& aMeshlets,
	const SC_Array<SGfx_PackedPrimitiveTriangle>& aPrimitives,
	const SC_Array<uint8>& aUniqueVertexIndices,
	const uint32 aVertexIndicesStride,
	SC_Array<uint8>& aOutIndexData,
	uint32& aOutIndexStride)
{
	uint32 numIndices = aPrimitives.Count() * 3;
	aOutIndexStride = (numIndices < SC_UINT16_MAX) ? sizeof(uint16) : sizeof(uint32);
	aOutIndexData.Respace(numIndices * aOutIndexStride);

	uint8* currentPos = aOutIndexData.GetBuffer();
	for (const SGfx_Meshlet& meshlet : aMeshlets)
	{
		const SGfx_PackedPrimitiveTriangle* primitives = &aPrimitives[meshlet.mPrimitiveOffset];
		const uint8* vertexIndices = &aUniqueVertexIndices[meshlet.mVertexOffset * aVertexIndicesStride];
		for (uint32 i = 0; i < meshlet.mPrimitiveCount; ++i)
		{
			const SGfx_PackedPrimitiveTriangle& triangle = primitives[i];
			const SC_IntVector triIndices = triangle.Unpack();
			if (aOutIndexStride == sizeof(uint16))
			{
				const uint16* typedVertexIndices = reinterpret_cast<const uint16*>(vertexIndices);
				uint16 indices[3] = { typedVertexIndices[triIndices.x], typedVertexIndices[triIndices.y], typedVertexIndices[triIndices.z] };
				SC_Memcpy(currentPos, indices, aOutIndexStride * 3);
			}
			else
			{
				const uint32* typedVertexIndices = reinterpret_cast<const uint32*>(vertexIndices);
				uint32 indices[3] = { typedVertexIndices[triIndices.x], typedVertexIndices[triIndices.y], typedVertexIndices[triIndices.z] };
				SC_Memcpy(currentPos, indices, aOutIndexStride * 3);
			}
			currentPos += aOutIndexStride * 3;
		}
	}

	return true;
}

template<class IndexType>
static bool AddCandidateToMeshlet(uint32 aMaxVertices, uint32 aMaxPrimitives, InlineMeshlet<IndexType>& aMeshlet, IndexType (&aTriangle)[3])
{
	if (aMeshlet.mUniqueVertexIndices.Count() == aMaxVertices)
		return false;
	if (aMeshlet.mPrimitiveIndices.Count() == aMaxPrimitives)
		return false;

	static constexpr uint32 gUndef = SC_UINT32_MAX;
	uint32 indices[3] = { gUndef, gUndef, gUndef };
	uint32 newCount = 3;

	for (uint32 i = 0; i < aMeshlet.mUniqueVertexIndices.Count(); ++i)
	{
		for (uint32 j = 0; j < 3; ++j)
		{
			if (aMeshlet.mUniqueVertexIndices[i] == aTriangle[j])
			{
				indices[j] = i;
				--newCount;
			}
		}
	}

	if (aMeshlet.mUniqueVertexIndices.Count() + newCount > aMaxVertices)
		return false;

	for (uint32 j = 0; j < 3; ++j)
	{
		if (indices[j] == gUndef)
		{
			indices[j] = static_cast<uint32>(aMeshlet.mUniqueVertexIndices.Count());
			aMeshlet.mUniqueVertexIndices.Add(aTriangle[j]);
		}
	}

	PackedTriangle& primitiveTriangle = aMeshlet.mPrimitiveIndices.Add();
	primitiveTriangle.mIndex0 = indices[0];
	primitiveTriangle.mIndex1 = indices[1];
	primitiveTriangle.mIndex2 = indices[2];
	return true;
}

static SC_Vector ComputeNormal(SC_Vector* aTriangle)
{
	SC_Vector p0 = aTriangle[0];
	SC_Vector p1 = aTriangle[1];
	SC_Vector p2 = aTriangle[2];

	SC_Vector v01 = p0 - p1;
	SC_Vector v02 = p0 - p2;

	return v01.Cross(v02).GetNormalized();
}

template <typename IndexType>
static uint32 ComputeReuse(const InlineMeshlet<IndexType>& aMeshlet, IndexType(&aTriIndices)[3])
{
	uint32 count = 0;
	for (uint32 i = 0; i < aMeshlet.mUniqueVertexIndices.Count(); ++i)
	{
		for (uint32 j = 0; j < 3; ++j)
		{
			if (aMeshlet.mUniqueVertexIndices[i] == aTriIndices[j])
				++count;
		}
	}

	return count;
}

template <typename IndexType>
static float ComputeScore(const InlineMeshlet<IndexType>& aMeshlet, const SC_Vector4& aSphere, const SC_Vector& aNormal, IndexType(&aTriIndices)[3], SC_Vector* aTriVerts)
{
	static constexpr float reuseWeight = 0.334f;
	static constexpr float locWeight = 0.333f;
	static constexpr float oriWeight = 0.333f;

	// Vertex reuse
	uint32 reuse = ComputeReuse(aMeshlet, aTriIndices);
	float reuseScore = 1.0f - ((float)reuse / 3.0f);

	// Distance from center point
	float maxSq = 0.f;
	for (uint32 i = 0; i < 3; ++i)
	{
		SC_Vector v = aSphere.XYZ() - aTriVerts[i];
		maxSq = SC_Max(maxSq, v.Length2());
	}
	float r = aSphere.w;
	float r2 = r * r;
	float locScore = SC_Math::Log2(maxSq / r2 + 1);

	// Angle between normal and meshlet cone axis
	SC_Vector n = ComputeNormal(aTriVerts);
	float d = n.Dot(aNormal);
	float oriScore = (-d + 1.0f) / 2.0f;

	return reuseWeight * reuseScore + locWeight * locScore + oriWeight * oriScore;
}

template <typename IndexType>
static bool IsMeshletFull(uint32 aMaxVertices, uint32 aMaxPrimitives, const InlineMeshlet<IndexType>& meshlet)
{
	SC_ASSERT(meshlet.mUniqueVertexIndices.Count() <= aMaxVertices);
	SC_ASSERT(meshlet.mPrimitiveIndices.Count() <= aMaxPrimitives);

	return (meshlet.mUniqueVertexIndices.Count() >= aMaxVertices) || (meshlet.mPrimitiveIndices.Count() >= aMaxPrimitives);
}

static SC_Vector4 MinimumBoundingSphere(SC_Vector* points, uint32 count)
{
	SC_ASSERT(points != nullptr && count != 0);

	// Find the min & max points indices along each axis.
	uint32 minAxis[3] = { 0, 0, 0 };
	uint32 maxAxis[3] = { 0, 0, 0 };

	for (uint32 i = 1; i < count; ++i)
	{
		float* point = (float*)(points + i);

		for (uint32 j = 0; j < 3; ++j)
		{
			float* min = (float*)(&points[minAxis[j]]);
			float* max = (float*)(&points[maxAxis[j]]);

			minAxis[j] = point[j] < min[j] ? i : minAxis[j];
			maxAxis[j] = point[j] > max[j] ? i : maxAxis[j];
		}
	}

	// Find axis with maximum span.
	float distSqMax = 0.0f;
	uint32 axis = 0;

	for (uint32 i = 0; i < 3u; ++i)
	{
		SC_Vector min = points[minAxis[i]];
		SC_Vector max = points[maxAxis[i]];

		float distSq = (max - min).Length2();
		if (distSq > distSqMax)
		{
			distSqMax = distSq;
			axis = i;
		}
	}

	// Calculate an initial starting center point & radius.
	SC_Vector p1 = points[minAxis[axis]];
	SC_Vector p2 = points[maxAxis[axis]];

	SC_Vector center = (p1 + p2) * 0.5f;
	float radius = (p2 - p1).Length() * 0.5f;
	float radiusSq = radius * radius;

	// Add all our points to bounding sphere expanding radius & recalculating center point as necessary.
	for (uint32 i = 0; i < count; ++i)
	{
		SC_Vector point = *(points + i);
		float distSq = (point - center).Length2();

		if (distSq > radiusSq)
		{
			float dist = SC_Math::Sqrt(distSq);
			float k = (radius / dist) * 0.5f + 0.5f;

			center = (center * k) + (point * (1.0f - k));
			radius = (radius + dist) * 0.5f;
		}
	}

	return SC_Vector4(center, radius);
}

template<class IndexType>
static void BuildMeshlets(
	uint32 aMaxVertices, 
	uint32 aMaxPrimitives, 
	const uint8* aVertexData, 
	const uint32 aNumVertices,
	const SR_VertexLayout& aVertexLayout, 
	const IndexType* aIndexData,
	const uint32 aNumIndices,
	SC_Array<InlineMeshlet<IndexType>>& aOutMeshlets)
{
	const uint32 indexCount = aNumIndices;
	const uint32 triCount = indexCount / 3;
	const uint32 vertexCount = aNumVertices;
	const uint32 vertexStride = aVertexLayout.GetVertexStrideSize();

	SC_Array<uint32> adjacencyList;
	adjacencyList.Respace(indexCount);
	BuildAdjacencyList(aIndexData, aNumIndices, aVertexData, aNumVertices, aVertexLayout, adjacencyList);

	aOutMeshlets.RemoveAll();
	InlineMeshlet<IndexType>* currentMeshlet = &aOutMeshlets.Add();

	std::vector<bool> checklist;
	checklist.resize(triCount);

	SC_Array<SC_Vector> positions;
	SC_Array<SC_Vector> normals;
	SC_Array<SC_Pair<uint32, float>> candidates;
	std::unordered_set<uint32> candidateCheck;

	SC_Vector4 pSphere;

	uint32 triangleIndex = 0;
	candidates.Add(SC_Pair(triangleIndex, 0.0f));
	candidateCheck.insert(triangleIndex);
	while (!candidates.IsEmpty())
	{
		uint32 index = candidates.Last().mFirst;
		candidates.RemoveLast();

		IndexType triangle[3] =
		{
			aIndexData[index * 3],
			aIndexData[index * 3 + 1],
			aIndexData[index * 3 + 2],
		};
		SC_ASSERT((triangle[0] < vertexCount) && (triangle[1] < vertexCount) && (triangle[2] < vertexCount));
		SC_UNUSED(vertexCount);
		
		if (AddCandidateToMeshlet(aMaxVertices, aMaxPrimitives, *currentMeshlet, triangle))
		{
			checklist[index] = true; 
			
			const SC_Vector* v0 = reinterpret_cast<const SC_Vector*>(&aVertexData[triangle[0] * vertexStride]);
			const SC_Vector* v1 = reinterpret_cast<const SC_Vector*>(&aVertexData[triangle[1] * vertexStride]);
			const SC_Vector* v2 = reinterpret_cast<const SC_Vector*>(&aVertexData[triangle[2] * vertexStride]);
			SC_Vector points[3] =
			{
				*v0,
				*v1,
				*v2,
			};
			positions.Add(points[0]);
			positions.Add(points[1]);
			positions.Add(points[2]);
			pSphere = MinimumBoundingSphere(positions.GetBuffer(), positions.Count());

			SC_Vector normal = ComputeNormal(points);
			normals.Add(normal);
			SC_Vector4 nSphere = MinimumBoundingSphere(normals.GetBuffer(), normals.Count());
			normal = nSphere.XYZ().GetNormalized();

			const uint32 adjIndex = index * 3;
			uint32 adj[3] =
			{
				adjacencyList[adjIndex],
				adjacencyList[adjIndex + 1],
				adjacencyList[adjIndex + 2],
			};
			for (uint32 i = 0; i < 3; ++i)
			{
				// Invalid triangle in adjacency slot
				if (adj[i] == SC_UINT32_MAX)
					continue;

				// Already processed triangle
				if (checklist[adj[i]])
					continue;

				// Triangle already in the candidate list
				if (candidateCheck.count(adj[i]))
					continue;

				candidates.Add(SC_Pair(adj[i], SC_FLT_MAX));
				candidateCheck.insert(adj[i]);
			}
			for (uint32 i = 0; i < candidates.Count(); ++i)
			{
				uint32 candidate = candidates[i].mFirst;

				IndexType triIndices[3] =
				{
					aIndexData[candidate * 3],
					aIndexData[candidate * 3 + 1],
					aIndexData[candidate * 3 + 2],
				};

				SC_ASSERT(triIndices[0] < vertexCount);
				SC_ASSERT(triIndices[1] < vertexCount);
				SC_ASSERT(triIndices[2] < vertexCount);

				const SC_Vector* tv0 = reinterpret_cast<const SC_Vector*>(&aVertexData[triIndices[0] * vertexStride]);
				const SC_Vector* tv1 = reinterpret_cast<const SC_Vector*>(&aVertexData[triIndices[1] * vertexStride]);
				const SC_Vector* tv2 = reinterpret_cast<const SC_Vector*>(&aVertexData[triIndices[2] * vertexStride]);
				SC_Vector triVerts[3] =
				{
					*tv0,
					*tv1,
					*tv2,
				};

				candidates[i].mSecond = ComputeScore(*currentMeshlet, pSphere, normal, triIndices, triVerts);
			}

			if (IsMeshletFull(aMaxVertices, aMaxPrimitives, *currentMeshlet))
			{
				positions.RemoveAll();
				normals.RemoveAll();
				candidateCheck.clear();

				// Use one of our existing candidates as the next meshlet seed.
				if (!candidates.IsEmpty())
				{
					candidates[0] = candidates.Last();
					candidates.Respace(1);
					candidateCheck.insert(candidates[0].mFirst);
				}

				currentMeshlet = &aOutMeshlets.Add();
			}
			else
			{
				std::stable_sort(candidates.begin(), candidates.end(), [](const SC_Pair<uint32,float>& a, const SC_Pair<uint32, float>& b) { return a.mSecond > b.mSecond; });
			}
		}
		else
		{
			if (candidates.IsEmpty())
			{
				positions.RemoveAll();
				normals.RemoveAll();
				candidateCheck.clear();

				currentMeshlet = &aOutMeshlets.Add();
			}
		}

		if (candidates.IsEmpty())
		{
			while (triangleIndex < triCount && checklist[triangleIndex])
				++triangleIndex;

			if (triangleIndex == triCount)
				break;

			candidates.Add(SC_Pair(triangleIndex, 0.0f));
			candidateCheck.insert(triangleIndex);
		}
	}

	if (aOutMeshlets.Last().mPrimitiveIndices.IsEmpty())
		aOutMeshlets.RemoveLast();
}

