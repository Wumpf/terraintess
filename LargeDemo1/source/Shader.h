#pragma once

class Shader
{
public:
	enum class Type
	{
		VERTEX,
		PIXEL,
		//HULL,
		//DOMAIN,
		//GEOMETRY
	};

	/// sets shader to the device
	virtual void Activate()=0;

	/// returns shader object
	/// \remarks casting to specialized shader object isn't meaningful since there are no additonal members for the specialised types!
	CComPtr<ID3D11DeviceChild> GetDeviceChild() { return _shader; }

	
	virtual ~Shader() {}

protected:
	Shader(ID3D11DeviceChild* shader, Type type) : _shader(shader), _type(type) {}

	static std::unique_ptr<char[]> LoadByteCodeFromFile(const std::string& filname, size_t& outFileLength);
	CComPtr<ID3D11DeviceChild> _shader;

private:
	Type _type;
};

class VertexShader : public Shader
{
public:
	/// create from compiled shader file
	static std::shared_ptr<VertexShader> FromFile(const std::string& filename, const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, unsigned int numInputLayoutElements);

	/// sets shader to the device
	/// \remarks will also set the input layout!
	void Activate();

	~VertexShader() {}
private:
	VertexShader(ID3D11DeviceChild* shader, ID3D11InputLayout* inputLayout) : Shader(shader, Type::VERTEX), _inputLayout(inputLayout) {}

	CComPtr<ID3D11InputLayout> _inputLayout;
};

class PixelShader : public Shader
{
public:
	/// create from compiled shader file
	static std::shared_ptr<PixelShader> FromFile(const std::string& filename);

	/// \copydoc Shader::Activate
	void Activate();

	~PixelShader() {}
private:
	PixelShader(ID3D11DeviceChild* shader) : Shader(shader, Type::PIXEL) {}
};
