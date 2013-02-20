#include "stdafx.h"
#include "Shader.h"
#include "DeviceManager.h"
#include "Utils.h"
#include <fstream>



std::unique_ptr<char[]> Shader::LoadByteCodeFromFile(const std::string& filename, int& outFileLength)
{
	std::ifstream shaderFile(filename, std::ios::binary | std::ios::ate);
	assert(!shaderFile.bad());
	if(shaderFile.bad())
		return nullptr;

	outFileLength = static_cast<int>(shaderFile.tellg());
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
	auto immediateContext = DeviceManager::Get().GetContext();
	immediateContext->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(_shader.p), nullptr, 0);
	immediateContext->IASetInputLayout(_inputLayout);
}

std::shared_ptr<VertexShader> VertexShader::FromFile(const std::string& filename, const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, unsigned int numInputLayoutElements)
{
	int fileLength; 
	std::unique_ptr<char[]> byteCode = LoadByteCodeFromFile(filename, fileLength);
	if(!byteCode.get())
		return nullptr;

	ID3D11VertexShader* shader;
	HRESULT hr = DeviceManager::Get().GetDevice()->CreateVertexShader(byteCode.get(), fileLength, nullptr, &shader);
	assert(SUCCEEDED(hr));
	ID3D11InputLayout* inputLayout;
	hr = DeviceManager::Get().GetDevice()->CreateInputLayout(inputLayoutDesc, numInputLayoutElements, byteCode.get(), fileLength, &inputLayout);
	assert(SUCCEEDED(hr));

	return std::shared_ptr<VertexShader>(new VertexShader(shader, inputLayout));
}


void PixelShader::Activate()
{
	DeviceManager::Get().GetContext()->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(_shader.p), nullptr, 0);
}

std::shared_ptr<PixelShader> PixelShader::FromFile(const std::string& filename)
{
	int fileLength; 
	std::unique_ptr<char[]> byteCode = LoadByteCodeFromFile(filename, fileLength);
	if(!byteCode.get())
		return nullptr;

	ID3D11PixelShader* shader;
	HRESULT hr = DeviceManager::Get().GetDevice()->CreatePixelShader(byteCode.get(), fileLength, nullptr, &shader);
	assert(SUCCEEDED(hr));

	return std::shared_ptr<PixelShader>(new PixelShader(shader));
}

void HullShader::Activate()
{
	DeviceManager::Get().GetContext()->HSSetShader(reinterpret_cast<ID3D11HullShader*>(_shader.p), nullptr, 0);
}

std::shared_ptr<HullShader> HullShader::FromFile(const std::string& filename)
{
	int fileLength; 
	std::unique_ptr<char[]> byteCode = LoadByteCodeFromFile(filename, fileLength);
	if(!byteCode.get())
		return nullptr;

	ID3D11HullShader* shader;
	HRESULT hr = DeviceManager::Get().GetDevice()->CreateHullShader(byteCode.get(), fileLength, nullptr, &shader);
	assert(SUCCEEDED(hr));

	return std::shared_ptr<HullShader>(new HullShader(shader));
}

void DomainShader::Activate()
{
	DeviceManager::Get().GetContext()->DSSetShader(reinterpret_cast<ID3D11DomainShader*>(_shader.p), nullptr, 0);
}

std::shared_ptr<DomainShader> DomainShader::FromFile(const std::string& filename)
{
	int fileLength; 
	std::unique_ptr<char[]> byteCode = LoadByteCodeFromFile(filename, fileLength);
	if(!byteCode.get())
		return nullptr;

	ID3D11DomainShader* shader;
	HRESULT hr = DeviceManager::Get().GetDevice()->CreateDomainShader(byteCode.get(), fileLength, nullptr, &shader);
	assert(SUCCEEDED(hr));

	return std::shared_ptr<DomainShader>(new DomainShader(shader));
}

void GeometryShader::Activate()
{
	DeviceManager::Get().GetContext()->GSSetShader(reinterpret_cast<ID3D11GeometryShader*>(_shader.p), nullptr, 0);
}

std::shared_ptr<GeometryShader> GeometryShader::FromFile(const std::string& filename)
{
	int fileLength; 
	std::unique_ptr<char[]> byteCode = LoadByteCodeFromFile(filename, fileLength);
	if(!byteCode.get())
		return nullptr;

	ID3D11GeometryShader* shader;
	HRESULT hr = DeviceManager::Get().GetDevice()->CreateGeometryShader(byteCode.get(), fileLength, nullptr, &shader);
	assert(SUCCEEDED(hr));

	return std::shared_ptr<GeometryShader>(new GeometryShader(shader));
}

void ComputeShader::Activate()
{
	DeviceManager::Get().GetContext()->CSSetShader(reinterpret_cast<ID3D11ComputeShader*>(_shader.p), nullptr, 0);
}

std::shared_ptr<ComputeShader> ComputeShader::FromFile(const std::string& filename)
{
	int fileLength; 
	std::unique_ptr<char[]> byteCode = LoadByteCodeFromFile(filename, fileLength);
	if(!byteCode.get())
		return nullptr;

	ID3D11ComputeShader* shader;
	HRESULT hr = DeviceManager::Get().GetDevice()->CreateComputeShader(byteCode.get(), fileLength, nullptr, &shader);
	assert(SUCCEEDED(hr));

	return std::shared_ptr<ComputeShader>(new ComputeShader(shader));
}