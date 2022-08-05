#pragma once

struct SGfx_IrradianceProbeGrid
{
	SC_IntVector mSize;
	float mCellSize;
};

class SGfx_GICascades
{

};

class SGfx_Raytracing
{
public:

	void BuildBVH();


	void ComputeGI();

private:
};

