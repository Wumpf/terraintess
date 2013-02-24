#pragma once

class BufferObject;
template<class T> class ConstantBuffer;

class Terrain
{
public:
	Terrain(float totalTerrainSize, unsigned int heightmapResolution, float _pixelPerTriangle, unsigned int patchCounterPerBlockSqrt = 8);
	~Terrain();

	void Draw(const class Camera& camera, float totalSize);

	void SetWireframe(bool wireframe) { _wireframe = wireframe; }
	bool GetWireframe() const { return _wireframe; }

private:
	void OnBackBufferResize(unsigned int height, unsigned int width);
	void DrawRecursive(const SimpleMath::Vector2& min, const SimpleMath::Vector2& max, const SimpleMath::Vector2& cameraPos2D);

	unsigned int _heightmapResolution;
	float _totalTerrainSize;
	float _minimumWorldBlockSize;

	float _pixelPerTriangle;

	bool _wireframe;


	std::shared_ptr<BufferObject> _blockVertexBuffer;	// todo rendering without vertex/index buffer
	std::shared_ptr<BufferObject> _blockIndexBuffer;

	std::shared_ptr<class Texture2D> _heightmapTexture;

	struct TerrainConstants
	{
		float HeightScale;		// max terrain height
		float HeightmapTexelSize;
		float HeightmapTexelSizeWorld_doubled;
		float TrianglesPerClipSpaceUnit;
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

	std::shared_ptr<class Effect> _effect;

	unsigned int _blockVertexCountSqrt;
};

