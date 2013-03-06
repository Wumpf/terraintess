#include "terrain_ds.hlsl"

float3 GetNormal(in float2 heightmapCoord, out float3 coarseNormal, out float3 detailNormal)
{
	// even better quality (?): use middle position sample (4 triangles!)

	// compute coarse normal
	float4 h;
	h[0] = CoarseHeightmap.Sample(SamplerTriLinearWrap, heightmapCoord + CoarseHeightmapTexelSize*float2( 0,-1)).r;
	h[1] = CoarseHeightmap.Sample(SamplerTriLinearWrap, heightmapCoord + CoarseHeightmapTexelSize*float2( 0, 1)).r;
	h[2] = CoarseHeightmap.Sample(SamplerTriLinearWrap, heightmapCoord + CoarseHeightmapTexelSize*float2( 1, 0)).r;
	h[3] = CoarseHeightmap.Sample(SamplerTriLinearWrap, heightmapCoord+ CoarseHeightmapTexelSize*float2(-1, 0)).r;
	h *= CoarseHeightScale;
	float3 vecdz = float3(0.0f, h[1] - h[0], CoarseHeightmapTexelSizeWorld_doubled);
	float3 vecdx = float3(CoarseHeightmapTexelSizeWorld_doubled, h[2] - h[3], 0.0f);
	coarseNormal = normalize(cross(vecdz, vecdx));

	// compute fine normal
	float2 detailHeightmapCoord = heightmapCoord * DetailHeightmapTexcoordFactor;
	h[0] = DetailHeightmap.Sample(SamplerTriLinearWrap, detailHeightmapCoord + DetailHeightmapTexelSize*float2( 0,-1)).r;
	h[1] = DetailHeightmap.Sample(SamplerTriLinearWrap, detailHeightmapCoord + DetailHeightmapTexelSize*float2( 0, 1)).r;
	h[2] = DetailHeightmap.Sample(SamplerTriLinearWrap, detailHeightmapCoord + DetailHeightmapTexelSize*float2( 1, 0)).r;
	h[3] = DetailHeightmap.Sample(SamplerTriLinearWrap, detailHeightmapCoord + DetailHeightmapTexelSize*float2(-1, 0)).r;
	h *= DetailHeightScale;
	vecdz = float3(0.0f, h[1] - h[0], DetailHeightmapTexelSizeWorld_doubled);
	vecdx = float3(DetailHeightmapTexelSizeWorld_doubled, h[2] - h[3], 0.0f);
	detailNormal = normalize(cross(vecdz, vecdx));

	// compute overall normal
	return normalize(coarseNormal + detailNormal);
}

float4 main(DS_OUTPUT input) : SV_TARGET
{
	// normals
	float3 coarseNormal, detailNormal;
	float3 normal = GetNormal(input.HeightmapCoord, coarseNormal, detailNormal);

	// texturing
	float3 textureTexcoord = input.WorldPos * TextureRepeat;
	float elevation = 1.0 - (coarseNormal.y * 0.75f + detailNormal.y * 0.25f);	// useful approx
	float rock = min(1, pow(elevation * 3.0f, 4.0f));
	float3 diffuseColor = lerp(DiffuseTextures.Sample(SamplerTriLinearWrap, float3(textureTexcoord.xz, 0)).rgb*1.5f,
								DiffuseTextures.Sample(SamplerTriLinearWrap, float3(textureTexcoord.xz, 2)).rgb, rock);

	if(input.WorldPos.y > 225)
	{
		float snow = saturate((input.WorldPos.y - 225.0f) / 50.0f - rock);
		diffuseColor = lerp(diffuseColor, DiffuseTextures.Sample(SamplerAnisotropicWrap, float3(textureTexcoord.xz, 3)).rgb, snow);
	}
	else if(input.WorldPos.y < 120)
	{
		float sand = saturate((120.0f - input.WorldPos.y) / 25.0f);
		diffuseColor = lerp(diffuseColor, DiffuseTextures.Sample(SamplerAnisotropicWrap, float3(textureTexcoord.xz, 1)).rgb, sand);
	}


	// shading
	float3 lightVec = normalize(float3(0.8f, 0.6f, 0.0f));
	float NDotL = dot(normal, lightVec);
	float fakeShadow = saturate(dot(coarseNormal, lightVec)*6);
	float lighting = max(0, NDotL) * fakeShadow * 0.7 + 0.3f;
	float3 outColor = diffuseColor * lighting;

	return float4(outColor, 1.0f);
}