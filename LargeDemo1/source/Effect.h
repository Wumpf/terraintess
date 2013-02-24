#pragma once

class Shader;

class Effect
{
public:
	
	Effect(const std::string& compiledVertexShaderFilename, const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, unsigned int numInputLayoutElements, const std::string& compiledPixelShaderFilename,
				const std::string& compiledGeometryShaderFilename = "", const std::string& compiledHullShaderFilename = "", const std::string& compiledDomainShaderFilename = "");
	Effect(const std::shared_ptr<Shader>& vertexShader, const std::shared_ptr<Shader>& pixelShader, 
					const std::shared_ptr<Shader>& geometryShader = nullptr, const std::shared_ptr<Shader>& hullShader = nullptr, const std::shared_ptr<Shader>& domainShader = nullptr);
	~Effect();

	void Activate();

private:
	std::shared_ptr<Shader> _vertexShader;
	std::shared_ptr<Shader> _hullShader;
	std::shared_ptr<Shader> _domainShader;
	std::shared_ptr<Shader> _geometryShader;
	std::shared_ptr<Shader> _pixelShader;
};

