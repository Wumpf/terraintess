#include "terrain_ds.hlsl"

float4 main(DS_OUTPUT input) : SV_TARGET
{
	// even better quality: use middle position sample (4 triangles!)

	// compute coarse normal
	float4 h;
	h[0] = CoarseHeightmap.Sample(SamplerAnisotropicWrap, input.HeightmapCoord + CoarseHeightmapTexelSize*float2( 0,-1)).r;
	h[1] = CoarseHeightmap.Sample(SamplerAnisotropicWrap, input.HeightmapCoord + CoarseHeightmapTexelSize*float2( 0, 1)).r;
	h[2] = CoarseHeightmap.Sample(SamplerAnisotropicWrap, input.HeightmapCoord + CoarseHeightmapTexelSize*float2( 1, 0)).r;
	h[3] = CoarseHeightmap.Sample(SamplerAnisotropicWrap, input.HeightmapCoord + CoarseHeightmapTexelSize*float2(-1, 0)).r;
	h *= CoarseHeightScale;
	float3 vecdz = float3(0.0f, h[1] - h[0], CoarseHeightmapTexelSizeWorld_doubled);
	float3 vecdx = float3(CoarseHeightmapTexelSizeWorld_doubled, h[2] - h[3], 0.0f);
	float3 coarseNormal = normalize(cross(vecdz, vecdx));

	// compute fine normal
	float2 detailHeightmapCoord = input.HeightmapCoord * DetailHeightmapTexcoordFactor;
	h[0] = DetailHeightmap.Sample(SamplerAnisotropicWrap, detailHeightmapCoord + DetailHeightmapTexelSize*float2( 0,-1)).r;
	h[1] = DetailHeightmap.Sample(SamplerAnisotropicWrap, detailHeightmapCoord + DetailHeightmapTexelSize*float2( 0, 1)).r;
	h[2] = DetailHeightmap.Sample(SamplerAnisotropicWrap, detailHeightmapCoord + DetailHeightmapTexelSize*float2( 1, 0)).r;
	h[3] = DetailHeightmap.Sample(SamplerAnisotropicWrap, detailHeightmapCoord + DetailHeightmapTexelSize*float2(-1, 0)).r;
	h *= CoarseHeightScale;
	vecdz = float3(0.0f, h[1] - h[0], DetailHeightmapTexelSizeWorld_doubled);
	vecdx = float3(DetailHeightmapTexelSizeWorld_doubled, h[2] - h[3], 0.0f);
	float3 detailNormal = normalize(cross(vecdz, vecdx));

	// compute overall normal
	float3 normal = normalize(coarseNormal + detailNormal);

	// shading

	float3 lightVec = normalize(float3(0.8f, 0.5f, 0.0f));
	float NDotL = dot(normal, lightVec)*0.8f + 0.2f;
	return float4(NDotL, NDotL, NDotL, 1.0f);
}