#include "terrain_hs.hlsl"

struct DS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 WorldPos : NORMAL;
	float2 HeightmapCoord : TEXCOORD0;
};

// as define to keep it type compatible
#define Bilerp(v0, v1, v2, v3, i)	lerp(lerp(v0, v1, i.x), lerp(v3, v2, i.x), i.y)

[domain("quad")]
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, float2 domain : SV_DomainLocation, const OutputPatch<HS_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT output;

	output.WorldPos = Bilerp(patch[0].WorldPos, patch[1].WorldPos, patch[2].WorldPos, patch[3].WorldPos, domain);
	output.HeightmapCoord = Bilerp(patch[0].HeightmapCoord, patch[1].HeightmapCoord, patch[2].HeightmapCoord, patch[3].HeightmapCoord, domain);


	/*float height_XY = Heightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord + float2(CoarseHeightmapTexelSize, CoarseHeightmapTexelSize), 0.0f).r;
	float height_Xy = Heightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord + float2(CoarseHeightmapTexelSize, -CoarseHeightmapTexelSize), 0.0f).r;
	float height_xY = Heightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord + float2(-CoarseHeightmapTexelSize, CoarseHeightmapTexelSize), 0.0f).r;
	float height_xy = Heightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord + float2(-CoarseHeightmapTexelSize, -CoarseHeightmapTexelSize), 0.0f).r;
	float2 heightmapFrac = frac(output.HeightmapCoord / CoarseHeightmapTexelSize);

	output.WorldPos.y = lerp(lerp(height_xy, height_Xy, heightmapFrac.x), 
							 lerp(height_xY, height_XY, heightmapFrac.x), heightmapFrac.y) * CoarseHeightScale;	

							 */


	// mipmap HeightmapTexcoordScale / input.InsideTessFactor[0] ? 
	output.WorldPos.y = CoarseHeightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord, 0.0f).r * CoarseHeightScale +
						DetailHeightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord * DetailHeightmapTexcoordFactor, 0.0f).r * DetailHeightScale;


	/**float4 h; 
	h[0] = Heightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord + CoarseHeightmapTexelSize*float2( 0,-1), 0.0f).r;
	h[1] = Heightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord + CoarseHeightmapTexelSize*float2( 0, 1), 0.0f).r;
	h[2] = Heightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord + CoarseHeightmapTexelSize*float2( 1, 0), 0.0f).r;
	h[3] = Heightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord + CoarseHeightmapTexelSize*float2(-1, 0), 0.0f).r;
	h *= CoarseHeightScale;
	float3 vecdz = float3(0.0f, h[1] - h[0], CoarseHeightmapTexelSizeWorld_doubled);
	float3 vecdx = float3(CoarseHeightmapTexelSizeWorld_doubled, h[2] - h[3], 0.0f);
	float3 normal = normalize(cross(vecdz, vecdx));
	float midVecBary =  float4(0.5f,0.5f,0.5f,0.5f) - domain;
	output.WorldPos.y -= normal.y * dot(midVecBary, midVecBary) * 3;
	*/
	output.Pos = mul(ViewProjection, float4(output.WorldPos, 1.0f));

	return output;
}
