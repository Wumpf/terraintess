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

float EstimateSphereSizeAroundEndge(const float3 p0, const float3 p1, float edgeLength)
{
	float3 edgeMid = (p1 + p0) * 0.5f;
	float2 edgeMidProjected = mul(ViewProjection, float4(edgeMid, 1.0f)).xw;
	float2 edgeUpProjected = mul(ViewProjection, float4(edgeMid + CameraX * edgeLength, 1.0f)).xw; 
	return abs(edgeMidProjected.x / edgeMidProjected.y - edgeUpProjected.x / edgeUpProjected.y);
};

bool InFrustum(const float3 p, const float margin)
{
	float3 cameraToPoint = p - CameraPosition;
	if(length(cameraToPoint) < margin)
		return true;

	float3 projectedToCameraViewDir = CameraZ * dot(cameraToPoint, CameraZ);
	float3 pointToProjectedToCameraViewDir = normalize(projectedToCameraViewDir - cameraToPoint);	// direction to point projected onto cameradirection

	// that is now the point nearest to the camera direction ("most visible")
	float3 realignedPoint = p + pointToProjectedToCameraViewDir * min(margin, length(pointToProjectedToCameraViewDir));
	float4 realignedPoint_projected = mul(ViewProjection, float4(realignedPoint, 1.0));
	float3 realignedPoint_clipSpace = realignedPoint_projected.xyz / realignedPoint_projected.w;

	if(realignedPoint_clipSpace.x < -1.0f || realignedPoint_clipSpace.x > 1.0f ||
	   realignedPoint_clipSpace.y < -1.0f || realignedPoint_clipSpace.y > 1.0f ||
	   realignedPoint_clipSpace.z < 0.0f || realignedPoint_clipSpace.z > 1.0f || 
	   realignedPoint_projected.w < 0.0f)
	{
		return false;
	}
	else
		return true;
};

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> ip, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// silhouettes? -> normal?

	// assume quadratic patches!
	float3 mid = 0.25f * (ip[0].WorldPos + ip[1].WorldPos + ip[2].WorldPos + ip[3].WorldPos);
	float edgeLength = ip[1].WorldPos.x - ip[0].WorldPos.x;	// should be positive
	float diagonalLength = sqrt(2) * edgeLength;

	if(InFrustum(mid, diagonalLength))
	{
		// estimate size on screen
		Output.EdgeTessFactor[0] = EstimateSphereSizeAroundEndge(ip[0].WorldPos, ip[3].WorldPos, edgeLength) * TrianglesPerClipSpaceUnit;
		Output.EdgeTessFactor[1] = EstimateSphereSizeAroundEndge(ip[1].WorldPos, ip[0].WorldPos, edgeLength) * TrianglesPerClipSpaceUnit;
		Output.EdgeTessFactor[2] = EstimateSphereSizeAroundEndge(ip[2].WorldPos, ip[1].WorldPos, edgeLength) * TrianglesPerClipSpaceUnit;
		Output.EdgeTessFactor[3] = EstimateSphereSizeAroundEndge(ip[3].WorldPos, ip[2].WorldPos, edgeLength) * TrianglesPerClipSpaceUnit;

		//Output.EdgeTessFactor[0] = Output.EdgeTessFactor[1] = Output.EdgeTessFactor[2] = Output.EdgeTessFactor[3] = 1;

		float midTess = (Output.EdgeTessFactor[0] + Output.EdgeTessFactor[1] + Output.EdgeTessFactor[2] + Output.EdgeTessFactor[3]) * 0.25f;
		Output.InsideTessFactor[0] = Output.InsideTessFactor[1] = midTess;
	}
	else
	{
		Output.EdgeTessFactor[0] = Output.EdgeTessFactor[1] = Output.EdgeTessFactor[2] = Output.EdgeTessFactor[3] = -1.0f;
		Output.InsideTessFactor[0] = Output.InsideTessFactor[1] = -1.0f;
	}

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
