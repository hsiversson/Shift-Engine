#include "SGfx_Shapes.h"

namespace SGfx_Shapes
{

	//--------------------------------------------------------------------------------
	// Icosahedron data
	//--------------------------------------------------------------------------------

	static constexpr float gIcoSphereX = 0.525731112119133606f;
	static constexpr float gIcoSphereZ = 0.850650808352039932f;
	static SC_Vector gIcoSphereVertices[12] =
	{
		{-gIcoSphereX,	0.0,			gIcoSphereZ},
		{ gIcoSphereX,	0.0,			gIcoSphereZ},
		{-gIcoSphereX,	0.0,		   -gIcoSphereZ},
		{ gIcoSphereX,	0.0,		   -gIcoSphereZ},
		{ 0.0,			gIcoSphereZ,	gIcoSphereX},
		{ 0.0,			gIcoSphereZ,   -gIcoSphereX},
		{ 0.0,		   -gIcoSphereZ,	gIcoSphereX},
		{ 0.0,		   -gIcoSphereZ,   -gIcoSphereX},
		{ gIcoSphereZ,	gIcoSphereX,	0.0},
		{-gIcoSphereZ,	gIcoSphereX,	0.0},
		{ gIcoSphereZ, -gIcoSphereX,	0.0},
		{-gIcoSphereZ, -gIcoSphereX,	0.0}
	};

	struct Triangle
	{
		uint32 mV0;
		uint32 mV1;
		uint32 mV2;
	};
	static Triangle gIcoSphereTriangles[20] =
	{
	   {0,4,1},  {0,9,4},  {9,5,4},  {4,5,8},  {4,8,1},
	   {8,10,1}, {8,3,10}, {5,3,8},  {5,2,3},  {2,7,3},
	   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
	   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5},  {7,2,11} };
	//--------------------------------------------------------------------------------

	static uint32 GetMiddleVertexIndex(SC_Array<SC_Vector>& aVertices, uint32 aV0, uint32 aV1, SC_UnorderedMap<uint64, uint32>& aMiddlePointCache, float aRadius, const SC_Vector& aCenter)
	{
		bool firstIsSmaller = aV0 < aV1;
		uint64 smallerIndex = firstIsSmaller ? aV0 : aV1;
		uint64 greaterIndex = firstIsSmaller ? aV1 : aV0;
		uint64 key = (smallerIndex << 32) + greaterIndex;

		if (aMiddlePointCache.find(key) != aMiddlePointCache.end())
			return aMiddlePointCache[key];

		const SC_Vector& v0 = aVertices[aV0];
		const SC_Vector& v1 = aVertices[aV1];
		SC_Vector newVertex = (v0 + v1) * 0.5f;
		newVertex.Normalize();
		aVertices.Add((newVertex * aRadius) + aCenter);
		uint32 newVertexIndex = aVertices.Count() - 1;

		aMiddlePointCache.insert(std::pair(key, newVertexIndex));
		return newVertexIndex;
	}

	bool GenerateSphere(SC_Array<SC_Vector>& aOutVertices, SC_Array<uint32>& aOutIndices, uint32 aSubdivisions, float aRadius, const SC_Vector& aCenter)
	{
		for (uint32 i = 0; i < 12; ++i)
		{
			aOutVertices.Add((gIcoSphereVertices[i].GetNormalized() * aRadius) + aCenter);
		}

		SC_Array<Triangle> triangles;
		triangles.Reserve(20);
		for (uint32 i = 0; i < 20; ++i)
		{
			triangles.Add(gIcoSphereTriangles[i]);
		}
			
		SC_UnorderedMap<uint64, uint32> middlePointCache;
		for (uint32 subdivideIndex = 0; subdivideIndex < aSubdivisions; ++subdivideIndex)
		{
			SC_Array<Triangle> triangles2;
			for (const Triangle& tri : triangles)
			{
				uint32 a = GetMiddleVertexIndex(aOutVertices, tri.mV0, tri.mV1, middlePointCache, aRadius, aCenter);
				uint32 b = GetMiddleVertexIndex(aOutVertices, tri.mV1, tri.mV2, middlePointCache, aRadius, aCenter);
				uint32 c = GetMiddleVertexIndex(aOutVertices, tri.mV2, tri.mV0, middlePointCache, aRadius, aCenter);

				triangles2.Add({ tri.mV0, a, c });
				triangles2.Add({ tri.mV1, b, a });
				triangles2.Add({ tri.mV2, c, b });
				triangles2.Add({ a, b, c });
			}

			triangles.Swap(triangles2);
		}

		for (const Triangle& tri : triangles)
		{
			aOutIndices.Add(tri.mV0);
			aOutIndices.Add(tri.mV1);
			aOutIndices.Add(tri.mV2);
		}

		return true;
	}

	bool GenerateCube(SC_Array<SC_Vector>& aOutVertices, SC_Array<uint16>& aOutIndices, const SC_Vector& aSize, const SC_Vector& aCenter)
	{
		const SC_Vector vertices[8] =
		{
			SC_Vector(-0.5f, -0.5f, -0.5f) * aSize + aCenter,
			SC_Vector(-0.5f, -0.5f,  0.5f) * aSize + aCenter,
			SC_Vector(-0.5f,  0.5f, -0.5f) * aSize + aCenter,
			SC_Vector(-0.5f,  0.5f,  0.5f) * aSize + aCenter,
			SC_Vector( 0.5f, -0.5f, -0.5f) * aSize + aCenter,
			SC_Vector( 0.5f, -0.5f,  0.5f) * aSize + aCenter,
			SC_Vector( 0.5f,  0.5f, -0.5f) * aSize + aCenter,
			SC_Vector( 0.5f,  0.5f,  0.5f) * aSize + aCenter,
		};
		aOutVertices.Reserve(8);
		aOutVertices.Add(vertices, 8);

		const uint16 indices[36] =
		{
			1,2,0,
			3,2,1,
			6,5,4,
			6,7,5,
			5,1,0,
			4,5,0,
			7,6,2,
			3,7,2,
			6,4,0,
			2,6,0,
			7,3,1,
			5,7,1
		};
		aOutIndices.Reserve(36);
		aOutIndices.Add(indices, 36);

		return true;
	}

}