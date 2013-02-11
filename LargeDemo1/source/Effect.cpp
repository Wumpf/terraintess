#include "stdafx.h"
#include "Effect.h"

#include "Shader.h"


Effect::Effect(const std::shared_ptr<Shader>& vertexShader, const std::shared_ptr<Shader>& pixelShader) :
	_vertexShader(vertexShader),
	_pixelShader(pixelShader)
{
}


Effect::Effect(const std::string& compiledVertexShaderFilename, const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, unsigned int numInputLayoutElements, const std::string& compiledPixelShaderFilename) :
	_vertexShader(VertexShader::FromFile(compiledVertexShaderFilename, inputLayoutDesc, numInputLayoutElements)),
	_pixelShader(PixelShader::FromFile(compiledPixelShaderFilename))
{
}
	

Effect::~Effect()
{
}

void Effect::Activate()
{
	_vertexShader->Activate();
	_pixelShader->Activate();
}