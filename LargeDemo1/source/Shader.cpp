#include "stdafx.h"
#include "Shader.h"
#include "DeviceManager.h"
#include "Utils.h"
#include <fstream>



std::unique_ptr<char[]> Shader::LoadByteCodeFromFile(const std::string& filename, size_t& outFileLength)
{
	std::ifstream shaderFile(filename, std::ios::binary | std::ios::ate);
	assert(!shaderFile.bad());
	if(shaderFile.bad())
		return nullptr;

	outFileLength = shaderFile.tellg();
	assert(outFileLength > 0);
	if(outFileLength <= 0)
		return nullptr;
	shaderFile.seekg(0, std::ios::beg);

	std::unique_ptr<char[]> buffer(new char[outFileLength]);
	shaderFile.read(buffer.get(), outFileLength);
	shaderFile.close();

	return buffer;
}

void VertexShader::Activate()
{
	auto immediateContext = DeviceManager::Get().GetImmediateContext();
	immediateContext->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(_shader.p), nullptr, 0);
	immediateContext->IASetInputLayout(_inputLayout);
}

void PixelShader::Activate()
{
	DeviceManager::Get().GetImmediateContext()->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(_shader.p), nullptr, 0);
}

std::shared_ptr<VertexShader> VertexShader::FromFile(const std::string& filename, const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, unsigned int numInputLayoutElements)
{
	size_t fileLength; 
	std::unique_ptr<char[]> byteCode = LoadByteCodeFromFile(filename, fileLength);
	if(!byteCode.get())
		return nullptr;

	ID3D11VertexShader* shader;
	assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateVertexShader(byteCode.get(), fileLength, nullptr, &shader)));
	ID3D11InputLayout* inputLayout;
	assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateInputLayout(inputLayoutDesc, numInputLayoutElements, byteCode.get(), fileLength, &inputLayout)));

	return std::shared_ptr<VertexShader>(new VertexShader(shader, inputLayout));
}

std::shared_ptr<PixelShader> PixelShader::FromFile(const std::string& filename)
{
	size_t fileLength; 
	std::unique_ptr<char[]> byteCode = LoadByteCodeFromFile(filename, fileLength);
	if(!byteCode.get())
		return nullptr;

	ID3D11PixelShader* shader;
	assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreatePixelShader(byteCode.get(), fileLength, nullptr, &shader)));

	return std::shared_ptr<PixelShader>(new PixelShader(shader));
}