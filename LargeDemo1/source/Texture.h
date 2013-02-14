#pragma once
class Texture2D
{
public:
	/// generates texture with full mipmapchain, no read/write, no aa, shader ressource
	template<class T> static std::unique_ptr<Texture2D> StandardTextureFromData(T* data, DXGI_FORMAT format, unsigned int width, unsigned int height)
	{ return StandardTextureFromData(data, sizeof(T), format, width, height); }
	static std::unique_ptr<Texture2D> StandardTextureFromData(void* data, unsigned int dataElementSize, DXGI_FORMAT format, unsigned int width, unsigned int height);

	CComPtr<ID3D11ShaderResourceView> GetRessourceView() { return _ressourceView; }

	~Texture2D() {}
private:
	Texture2D() {}

	CComPtr<ID3D11Texture2D> _texture;
	CComPtr<ID3D11ShaderResourceView> _ressourceView;
};

