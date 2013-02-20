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
	float3 WorldPos : POSITION;
	float2 HeightmapCoord : TEXCOORD0;
};


#define NUM_CONTROL_POINTS 4

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> ip, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// estimate size on screen
	float screenSize = max(abs(ip[0].WorldPosOnScreen.x - ip[1].WorldPosOnScreen.x), abs(ip[2].WorldPosOnScreen.y - ip[3].WorldPosOnScreen.y));

	Output.EdgeTessFactor[0] = length(ip[1].WorldPosOnScreen - ip[0].WorldPosOnScreen) * 80.0f;
	Output.EdgeTessFactor[1] = length(ip[2].WorldPosOnScreen - ip[1].WorldPosOnScreen) * 80.0f;
	Output.EdgeTessFactor[2] = length(ip[3].WorldPosOnScreen - ip[2].WorldPosOnScreen) * 80.0f;
	Output.EdgeTessFactor[3] = length(ip[0].WorldPosOnScreen - ip[3].WorldPosOnScreen) * 80.0f;

	float midTess = (Output.EdgeTessFactor[0] + Output.EdgeTessFactor[1] + Output.EdgeTessFactor[2] + Output.EdgeTessFactor[3]) * 0.25f;
	Output.InsideTessFactor[0] = Output.InsideTessFactor[1] = midTess;

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

	Output.WorldPos = ip[i].WorldPos;
	Output.HeightmapCoord = ip[i].HeightmapCoord;

	return Output;
}
