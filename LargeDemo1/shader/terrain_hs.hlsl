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

float EstimateSphereSizeAroundEndge(float3 p0, float3 p1)
{
	float edgeLength = length(p1 - p0);
	float3 edgeMid = (p1 + p0) * 0.5f;
	float2 edgeMidProjected = mul(ViewProjection, float4(edgeMid, 1.0f)).xw;
	float2 edgeUpProjected = mul(ViewProjection, float4(edgeMid + View[0].xyz, 1.0f)).xw; 
	return abs(edgeMidProjected.x / edgeMidProjected.y - edgeUpProjected.x / edgeUpProjected.y);
}

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> ip, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// estimate size on screen
	Output.EdgeTessFactor[0] = EstimateSphereSizeAroundEndge(ip[0].WorldPos, ip[3].WorldPos) * TesselationFactor;
	Output.EdgeTessFactor[1] = EstimateSphereSizeAroundEndge(ip[1].WorldPos, ip[0].WorldPos) * TesselationFactor;
	Output.EdgeTessFactor[2] = EstimateSphereSizeAroundEndge(ip[2].WorldPos, ip[1].WorldPos) * TesselationFactor;
	Output.EdgeTessFactor[3] = EstimateSphereSizeAroundEndge(ip[3].WorldPos, ip[2].WorldPos) * TesselationFactor;

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
