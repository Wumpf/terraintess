#pragma once

class Shader;

class Cube
{
public:
	Cube();
	~Cube();

	void Draw();

private:
	CComPtr<ID3D11Buffer> _vertexBuffer;
	CComPtr<ID3D11Buffer> _indexBuffer;
	std::unique_ptr<class Effect> _effect;
};

