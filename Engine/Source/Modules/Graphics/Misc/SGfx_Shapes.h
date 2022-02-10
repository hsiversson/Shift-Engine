#pragma once



namespace SGfx_Shapes
{

	bool GenerateSphere(SC_Array<SC_Vector>& aOutVertices, SC_Array<uint32>& aOutIndices, uint32 aSubdivisions = 1, float aRadius = 1.0f, const SC_Vector& aCenter = SC_Vector(0));
	bool GenerateCube(SC_Array<SC_Vector>& aOutVertices, SC_Array<uint16>& aOutIndices, const SC_Vector& aSize = SC_Vector(1), const SC_Vector& aCenter = SC_Vector(0));

}
