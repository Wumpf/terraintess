cbuffer TerrainConstants : register(b0)
{
	float CoarseHeightScale						: packoffset(c0.x);		// max terrain height
	float CoarseHeightmapTexelSize				: packoffset(c0.y);	
	float CoarseHeightmapTexelSizeWorld_doubled	: packoffset(c0.z);	// size of a texel in worldcoordinates doubled
	float TrianglesPerClipSpaceUnit				: packoffset(c0.w);

	float DetailHeightScale						: packoffset(c1.x);
	float DetailHeightmapTexcoordFactor			: packoffset(c1.y);
	float DetailHeightmapTexelSize				: packoffset(c1.z);	
	float DetailHeightmapTexelSizeWorld_doubled	: packoffset(c1.w);	// size of a texel in worldcoordinates doubled
}

cbuffer PatchConstants : register(b1)
{
	float2 WorldPosition				: packoffset(c0.x);		// 2D Position in world
	float2 HeightmapTexcoordPosition	: packoffset(c0.z);		// 2D Position on heightmap

	float PlaneScale					: packoffset(c1.x);		// Scale of the Patch
	float HeightmapTexcoordScale		: packoffset(c1.y);		// size of the patch on the heightmap
};

Texture2D CoarseHeightmap : register(t0);
Texture2D DetailHeightmap : register(t1);