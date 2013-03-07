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
	float2 WorldPos2D : POSITION;
	float2 HeightmapCoord : TEXCOORD0;
	float SkirtFactor : SKIRT;
};


#define NUM_CONTROL_POINTS 4

float EstimateSphereSizeAroundEndge(const float3 p0, const float3 p1)
{
	float edgeLength = distance(p0, p1);
	float3 edgeMid = (p1 + p0) * 0.5f;
	float2 edgeMidProjected = mul(ViewProjection, float4(edgeMid, 1.0f)).xw;
	float2 edgeUpProjected = mul(ViewProjection, float4(edgeMid + CameraX * edgeLength, 1.0f)).xw; 
	return abs(edgeMidProjected.x / edgeMidProjected.y - edgeUpProjected.x / edgeUpProjected.y);
};

bool InFrustum(const float3 p, const float margin)
{
	// function based on NVIDIA's TerrainTessellation Sample

	float3 cameraToPoint = p - CameraPosition;
	if(length(cameraToPoint) < margin)
		return true;

	float3 projectedToCameraViewDir = CameraZ * dot(cameraToPoint, CameraZ);
	float3 pointToCameraViewDir = projectedToCameraViewDir - cameraToPoint;	// direction to point projected onto cameradirection
	float pointToCameraViewDir_length = length(pointToCameraViewDir);
	pointToCameraViewDir /= pointToCameraViewDir_length;

	// that is now the point nearest to the camera direction ("most visible")
	float3 realignedPoint = p + pointToCameraViewDir * min(margin, pointToCameraViewDir_length);
	float4 realignedPoint_projected = mul(ViewProjection, float4(realignedPoint, 1.0f));

	if(realignedPoint_projected.x < -realignedPoint_projected.w || realignedPoint_projected.x > realignedPoint_projected.w ||
	   realignedPoint_projected.y < -realignedPoint_projected.w || realignedPoint_projected.y > realignedPoint_projected.w ||
		realignedPoint_projected.z < 0.0f || realignedPoint_projected.z > realignedPoint_projected.w)
	{
		return false;
	}
	
	return true;
};

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> ip, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// silhouettes? -> normal?

	// assume quadratic patches!
	float3 mid = 0.25f * (ip[0].WorldPos + ip[1].WorldPos + ip[2].WorldPos + ip[3].WorldPos);
	float diagonalLength = max(distance(ip[0].WorldPos, ip[2].WorldPos), distance(ip[1].WorldPos, ip[3].WorldPos));	// add offset

	if(InFrustum(mid, diagonalLength))
	{
		Output.InsideTessFactor[0] = 0.0f;
		bool anySkirt = false;

		[unroll]for(int i=0; i<4; ++i)
		{
			int second = (i+3) % 4;

			/*if(ip[i].SkirtFactor > 0.0001f || ip[second].SkirtFactor > 0.0001f)
			{
				Output.EdgeTessFactor[i] = 1.0f;
				anySkirt = true;
			}
			else */
				Output.EdgeTessFactor[i] = EstimateSphereSizeAroundEndge(ip[i].WorldPos, ip[second].WorldPos) * TrianglesPerClipSpaceUnit;
		}
	/*	if(anySkirt)
			Output.InsideTessFactor[1] = 1.0f;
		else */
			Output.InsideTessFactor[1] = (Output.EdgeTessFactor[0]+Output.EdgeTessFactor[1]+Output.EdgeTessFactor[2]+Output.EdgeTessFactor[3]) * 0.25f;
		Output.InsideTessFactor[0] = Output.InsideTessFactor[1];
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

	Output.WorldPos2D = ip[i].WorldPos.xz;
	Output.HeightmapCoord = ip[i].HeightmapCoord;
	Output.SkirtFactor = ip[i].SkirtFactor;

	return Output;
}
