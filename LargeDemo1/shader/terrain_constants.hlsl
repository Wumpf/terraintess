cbuffer TerrainConstants :  register(b0)
{
	float HeightScale			: packoffset(c0.x);		// max terrain height
	float HeightmapTexelSize	: packoffset(c0.y);	
	float HeightmapTexelSizeWorld_doubled : packoffset(c0.z);	// size of a texel in worldcoordinates doubled
	float TrianglesPerClipSpaceUnit : packoffset(c0.w);
}

cbuffer PatchConstants : register(b1)
{
	float2 WorldPosition				: packoffset(c0.x);		// 2D Position in world
	float2 HeightmapTexcoordPosition	: packoffset(c0.z);	// 2D Position on heightmap

	float PlaneScale					: packoffset(c1.x);		// Scale of the Patch
	float HeightmapTexcoordScale		: packoffset(c1.y);		// size of the patch on the heightmap
};

Texture2D Heightmap : register(t0);