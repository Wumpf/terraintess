#pragma once

class BufferObject;
template<class T> class ConstantBuffer;

class Terrain
{
public:

	///
	/// \param totalTerrainSize				total terrain size in world
	/// \param heightmapResolution			resolution of the coarse heightmap in pixel
	/// \param detailHeightmapWorldSize		size of the detail heightmap in pixel (resolution is fixed)
	/// \param heightmapYScale				height-scale of the coarse heightmap
	/// \param detailHeightmapYScale		height-scale of the detail heightmap
	Terrain(float totalTerrainSize, unsigned int heightmapResolution, float pixelPerTriangle, 
				float DetailHeightmapTexcoordFactor = 50.0f, float heightmapYScale = 400.0f, float detailHeightmapYScale = 8.0f, 
				unsigned int patchCountPerBlockSqrt = 8);
	~Terrain();

	void Draw(const class Camera& camera, float totalSize);

	void SetWireframe(bool wireframe) { _wireframe = wireframe; }
	bool GetWireframe() const { return _wireframe; }

	void OnBackBufferResize(unsigned int width, unsigned int height);

private:
	void DrawRecursive(const SimpleMath::Vector2& min, const SimpleMath::Vector2& max, const SimpleMath::Vector2& cameraPos2D);

	unsigned int _heightmapResolution;
	float _totalTerrainSize;
	float _minimumWorldBlockSize;
	float _pixelPerTriangle;

	std::shared_ptr<BufferObject> _blockVertexBuffer;	// todo rendering without vertex/index buffer
	std::shared_ptr<BufferObject> _blockIndexBuffer;

	std::shared_ptr<class Effect> _effect;

	std::shared_ptr<class Texture2D> _heightmapCoarseTexture;
	std::shared_ptr<class Texture2D> _heightmapDetailTexture;

	struct TerrainConstants
	{
		float CoarseHeightScale;		// max terrain height
		float CoarseHeightmapTexelSize;
		float CoarseHeightmapTexelSizeWorld_doubled;
		float TrianglesPerClipSpaceUnit;

		float DetailHeightScale;
		float DetailHeightmapTexcoordFactor;
		float DetailHeightmapTexelSize;
		float DetailHeightmapTexelSizeWorld_doubled;
	};
	std::unique_ptr<ConstantBuffer<TerrainConstants>> _terrainConstantBuffer;

	struct PatchConstants
	{
		SimpleMath::Vector2 WorldPosition;		// 2D Position in world
		SimpleMath::Vector2 HeightmapTexcoordPosition;	// 2D Position on heightmap

		float PlaneScale;		// Scale of the Patch
		float HeightmapTexcoordScale;		// size of the patch on the heightmap
	};
	std::unique_ptr<ConstantBuffer<PatchConstants>> _patchConstantBuffer;


	bool _wireframe;

	static const unsigned int DETAIL_HEIGHTMAP_RESOLUTION = 512;
};

