#pragma once

class BufferObject;

class Terrain
{
public:
	Terrain(unsigned int blockVertexCountSqrt);
	~Terrain();

	void Draw(const class Camera& camera, float totalSize);

private:
	std::shared_ptr<BufferObject> _blockVertexBuffer;
	std::shared_ptr<BufferObject> _blockIndexBuffer;

	std::shared_ptr<BufferObject> _immutableConstantBuffer;
	std::shared_ptr<BufferObject> _perFrameConstantBuffer;

	std::shared_ptr<class Texture2D> _heightmapTexture;

	std::shared_ptr<class Effect> _effect;

	unsigned int _blockVertexCountSqrt;
};

