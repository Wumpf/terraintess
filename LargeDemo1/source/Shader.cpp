#include "stdafx.h"
#include "Shader.h"
#include "Direct3D11Device.h"
#include "Utils.h"
#include <fstream>


Shader::~Shader(void)
{
	Utils::SafeReleaseCOM(_shader);
}

std::unique_ptr<Shader> Shader::FromFile(std::string& filename, Type type)
{
	std::ifstream shaderFile(filename, std::ios::binary);
	if(shaderFile.bad())
		return nullptr;

	shaderFile.seekg(0, std::ios::end);
	std::streamoff fileLength = shaderFile.tellg();
	shaderFile.seekg(0, std::ios::beg);

	std::unique_ptr<char[]> buffer(new char[fileLength]);
	shaderFile.read(buffer.get(), fileLength);
	shaderFile.close();

	std::unique_ptr<Shader> shader(new Shader());
	shader->_shaderType = type;
	switch(type)
	{
	case Type::PIXEL:
		if(FAILED(Direct3D11Device::Get().GetDevice()->CreatePixelShader(buffer.get(), fileLength, 
						nullptr, reinterpret_cast<ID3D11PixelShader**>(&shader->_shader))))
			return nullptr;

	case Type::VERTEX:
		if(FAILED(Direct3D11Device::Get().GetDevice()->CreateVertexShader(buffer.get(), fileLength, 
						nullptr, reinterpret_cast<ID3D11VertexShader**>(&shader->_shader))))
			return nullptr;
	}
	
	return shader;
}

void Shader::Activate()
{
	switch(_shaderType)
	{
	case Type::PIXEL:
		Direct3D11Device::Get().GetImmediateContext()->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(_shader), nullptr, 0);
		break;
	case Type::VERTEX:
		Direct3D11Device::Get().GetImmediateContext()->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(_shader), nullptr, 0);
		break;
	}
}