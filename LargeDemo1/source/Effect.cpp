#include "stdafx.h"
#include "Effect.h"
#include "DeviceManager.h"
#include "Shader.h"


Effect::Effect(const std::shared_ptr<Shader>& vertexShader, const std::shared_ptr<Shader>& pixelShader, 
				const std::shared_ptr<Shader>& geometryShader, const std::shared_ptr<Shader>& hullShader, const std::shared_ptr<Shader>& domainShader) :
	_vertexShader(vertexShader),
	_geometryShader(geometryShader),
	_hullShader(hullShader),
	_domainShader(domainShader),
	_pixelShader(pixelShader)
{
}

Effect::Effect(const std::string& compiledVertexShaderFilename, const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, unsigned int numInputLayoutElements, const std::string& compiledPixelShaderFilename,
			   const std::string& compiledGeometryShaderFilename, const std::string& compiledHullShaderFilename, const std::string& compiledDomainShaderFilename) :
	_vertexShader(VertexShader::FromFile(compiledVertexShaderFilename, inputLayoutDesc, numInputLayoutElements)),
	_pixelShader(PixelShader::FromFile(compiledPixelShaderFilename))
{
	if(compiledGeometryShaderFilename != "")
		_geometryShader = GeometryShader::FromFile(compiledGeometryShaderFilename);
	if(compiledHullShaderFilename != "")
		_hullShader = HullShader::FromFile(compiledHullShaderFilename);
	if(compiledDomainShaderFilename != "")
		_domainShader = DomainShader::FromFile(compiledDomainShaderFilename);
}
	

Effect::~Effect()
{
}

void Effect::Activate()
{
	_vertexShader->Activate();
	_pixelShader->Activate();
	if(_geometryShader.get())
		_geometryShader->Activate();
	else
		DeviceManager::Get().GetImmediateContext()->GSSetShader(nullptr, nullptr, 0);
	if(_hullShader.get())
		_hullShader->Activate();
	else
		DeviceManager::Get().GetImmediateContext()->HSSetShader(nullptr, nullptr, 0);
	if(_domainShader.get())
		_domainShader->Activate();
	else
		DeviceManager::Get().GetImmediateContext()->DSSetShader(nullptr, nullptr, 0);
}