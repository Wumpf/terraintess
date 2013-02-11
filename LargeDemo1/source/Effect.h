#pragma once

class Shader;

class Effect
{
public:
	Effect(const std::shared_ptr<Shader>& vertexShader, const std::shared_ptr<Shader>& pixelShader);
	Effect(const std::string& compiledVertexShaderFilename, const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, unsigned int numInputLayoutElements, const std::string& compiledPixelShaderFilename);
	~Effect();

	void Activate();

private:
	std::shared_ptr<Shader> _vertexShader;
	std::shared_ptr<Shader> _pixelShader;
};

