#pragma once

class Terrain
{
public:
	Terrain();
	~Terrain();

	void Draw(const class Camera& camera);

private:
	CComPtr<ID3D11Buffer> _blockVertexBuffer;
	CComPtr<ID3D11Buffer> _blockIndexBuffer;

	CComPtr<ID3D11Buffer> _immutableConstantBuffer;
	CComPtr<ID3D11Buffer> _perFrameConstantBuffer;

	std::unique_ptr<class Effect> _effect;
};

