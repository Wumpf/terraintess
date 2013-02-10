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

	static std::unique_ptr<Shader> FromFile(std::string& filename, Type type);

	void Activate();


	virtual ~Shader();
private:
	Shader() {}

	ID3D11DeviceChild* _shader;
	Type _shaderType;
};
