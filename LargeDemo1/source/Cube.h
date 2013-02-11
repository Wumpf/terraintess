#pragma once

class Shader;

class Cube
{
public:
	Cube();
	~Cube();

	void Draw(const class Camera& camera, float totalPassedTime);

private:
	CComPtr<ID3D11Buffer> _vertexBuffer;
	CComPtr<ID3D11Buffer> _indexBuffer;
	CComPtr<ID3D11Buffer> _constantBuffer;

	std::unique_ptr<class Effect> _effect;
};

