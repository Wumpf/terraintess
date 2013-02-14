#include "terrain_vs.hlsl"

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4]			: SV_TessFactor;
	float InsideTessFactor[2]		: SV_InsideTessFactor;
};

// Output control point
struct HS_OUTPUT
{
	float2 Pos2D : POSITION;
};


#define NUM_CONTROL_POINTS 4

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> ip, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	Output.EdgeTessFactor[0] = 
		Output.EdgeTessFactor[1] = 
		Output.EdgeTessFactor[2] = 
		Output.EdgeTessFactor[3] = 
		Output.InsideTessFactor[0] =
		Output.InsideTessFactor[1] = 1.0f;

	return Output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_OUTPUT main(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> ip, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
	HS_OUTPUT Output;

	Output.Pos2D = ip[i].Pos2D;

	return Output;
}
