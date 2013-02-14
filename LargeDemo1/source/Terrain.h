#pragma once

class Terrain
{
public:
	Terrain(unsigned int blockVertexCountSqrt);
	~Terrain();

	void Draw(const class Camera& camera, float totalSize);

private:
	CComPtr<ID3D11Buffer> _blockVertexBuffer;
	unsigned int _numIndices;
	CComPtr<ID3D11Buffer> _blockIndexBuffer;

	CComPtr<ID3D11Buffer> _immutableConstantBuffer;
	CComPtr<ID3D11Buffer> _perFrameConstantBuffer;

	std::unique_ptr<class Texture2D> _heightmapTexture;

	std::unique_ptr<class Effect> _effect;

	unsigned int _blockVertexCountSqrt;
};

