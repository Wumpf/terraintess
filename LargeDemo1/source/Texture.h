#pragma once

#include "Utils.h"

class Texture2D
{
public:
	enum class CreationFlags : uint32_t
	{
		FULL_MIPMAPCHAIN	 = 0xF0000000u,
		AUTOGEN_MIPMAPS		 = 0x0F000000u,	// needs full mipmap chain, cpu access ("deferred fill") and srv and rendertarget
		//CPU_WRITE			 = 0x00F00000u,
		//CPU_READ			 = 0x0000000Fu,
		SHADERRES_VIEW		 = 0x000F0000u,
		RENDERTARGET_VIEW	 = 0x0000F000u,
		UNORDEREDACCESS_VIEW = 0x00000F00u
	};

	static std::shared_ptr<Texture2D> CreateEmpty(DXGI_FORMAT format, unsigned int width, unsigned int height,
													CreationFlags creationFlags = CreationFlags::SHADERRES_VIEW); 
												  
	static std::shared_ptr<Texture2D> CreateEmpty(DXGI_FORMAT format, unsigned int width, unsigned int height, DXGI_SAMPLE_DESC& samplingSettings,
													CreationFlags creationFlags = CreationFlags::SHADERRES_VIEW);

	static std::shared_ptr<Texture2D> CreateFromData(const void* data, DXGI_FORMAT format, unsigned int width, unsigned int height, 
													CreationFlags creationFlags = CreationFlags::AUTOGEN_MIPMAPS);

	static std::shared_ptr<Texture2D> CreateEx(const void* data, DXGI_FORMAT format, unsigned int width, unsigned int height, DXGI_SAMPLE_DESC& samplingSettings, CreationFlags creationFlags);




	/// get view for shader resource
	/// \brief returns nullptr if no shader ressource view available
	CComPtr<ID3D11ShaderResourceView> GetShaderResourceView()	{ return _shaderResourceView; }
	
	/// get view for unordered access
	/// \brief returns nullptr if no unordered access view available
	CComPtr<ID3D11UnorderedAccessView> GetUnorderedAcessView()	{ return _unorderedAccessView; }

	/// get view for rendertarget usage
	/// \brief returns nullptr if no render target view available
	CComPtr<ID3D11RenderTargetView> GetRenderTargetView()		{ return _renderTargetView; }

	~Texture2D() {}
private:
	Texture2D() {}

	CComPtr<ID3D11Texture2D> _texture;

	// possible views
	CComPtr<ID3D11ShaderResourceView>	_shaderResourceView;
	CComPtr<ID3D11UnorderedAccessView>	_unorderedAccessView;
	CComPtr<ID3D11RenderTargetView>		_renderTargetView;
};

CLASS_ENUM_FLAG(Texture2D::CreationFlags)