#include "terrain_ds.hlsl"

float4 main(DS_OUTPUT input) : SV_TARGET
{
	// even better quality: use middle position sample (4 triangles!)
	float4 h; 
	h[0] = Heightmap.Sample(SamplerTriLinearWrap, input.HeightmapCoord + HeightmapTexelSize*float2( 0,-1)).r;
	h[1] = Heightmap.Sample(SamplerTriLinearWrap, input.HeightmapCoord + HeightmapTexelSize*float2( 0, 1)).r;
	h[2] = Heightmap.Sample(SamplerTriLinearWrap, input.HeightmapCoord + HeightmapTexelSize*float2( 1, 0)).r;
	h[3] = Heightmap.Sample(SamplerTriLinearWrap, input.HeightmapCoord + HeightmapTexelSize*float2(-1, 0)).r;
	h *= HeightScale;
	float3 vecdz = float3(0.0f, h[1] - h[0], HeightmapTexelSizeWorld_doubled);
	float3 vecdx = float3(HeightmapTexelSizeWorld_doubled, h[2] - h[3], 0.0f);
	float3 normal = normalize(cross(vecdz, vecdx));

	float3 lightVec = normalize(float3(0.8f, 0.5f, 0.0f));
	float NDotL = dot(normal, lightVec);
	return float4(NDotL, NDotL, NDotL, 1.0f);
}