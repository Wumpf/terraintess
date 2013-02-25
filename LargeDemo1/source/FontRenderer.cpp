#include "stdafx.h"
#include "FontRenderer.h"
#include "Effect.h"
#include "BufferObject.h"
#include "FontSheet.h"
#include "DeviceManager.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "Texture.h"

const D3D11_INPUT_ELEMENT_DESC FontRenderer::CharacterVertex::desc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "WIDTH", 0, DXGI_FORMAT_R32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
const unsigned int FontRenderer::CharacterVertex::numDescElements = sizeof(FontRenderer::CharacterVertex::desc) / sizeof(FontRenderer::CharacterVertex::desc[0]);

FontRenderer::FontRenderer() :
	_fontVertexBuffer(new BufferObject(nullptr, MAX_CHARS_PER_DRAW, sizeof(CharacterVertex), D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC)),
	_fontShaders(new Effect("shader/font_vs.cso", CharacterVertex::desc, CharacterVertex::numDescElements, "shader/font_ps.cso", "shader/font_gs.cso")),
	_constantBuffer(new ConstantBuffer<FontSettings>())
{

}


FontRenderer::~FontRenderer()
{
}

void FontRenderer::DrawString(class FontSheet& fontSheet, const std::string text, const SimpleMath::Vector2& textPosition_pixel, const SimpleMath::Color& color, float marginFactor)
{
	// another interessting variant would be to give the graphics card only characters an the full atlas
	// but since we need to compute the individual char positions this doesn't seems that much attractive

	float charMargin = fontSheet.GetSpaceWidth() * marginFactor;

	// common draw settings
	auto context = DeviceManager::Get().GetContext();
	_fontShaders->Activate();
	UINT stride = sizeof(CharacterVertex);
    UINT offset = 0;
	context->IASetVertexBuffers(0, 1, _fontVertexBuffer->GetBufferPointer(), &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	DeviceManager::Get().SetRasterizerState(RasterizerState::CullNone);
	DeviceManager::Get().SetBlendState(BlendState::AlphaBlend);

	auto resView = fontSheet.GetFontTexture()->GetShaderResourceView().p;
	context->PSSetShaderResources(0, 1, &resView);

	// constant buffer
	FontSettings& fontSettings = _constantBuffer->GetContent();
	fontSettings.Height_clipSpace = 2.0f * fontSheet.GetCharHeight() / DeviceManager::Get().GetBackBufferHeight();
	fontSettings.FontAtlasSize.x = static_cast<float>(fontSheet.GetFontTextureWidth());
	fontSettings.FontAtlasSize.y = static_cast<float>(fontSheet.GetFontTextureHeight());
	fontSettings.Height_texSpace = fontSheet.GetCharHeight() / fontSettings.FontAtlasSize.y;
	fontSettings.Color = color;
	_constantBuffer->UpdateGPUBuffer();
	context->GSSetConstantBuffers(0, 1, _constantBuffer->GetBufferPointer());
	context->PSSetConstantBuffers(0, 1, _constantBuffer->GetBufferPointer());

	// traverse string
	size_t startChar = 0;
	size_t endChar = 0;
	SimpleMath::Vector2 screenPos = textPosition_pixel;

	while(endChar < text.length())
	{
		if(text.length() - endChar < MAX_CHARS_PER_DRAW)
			endChar = text.length();
		else
			endChar = MAX_CHARS_PER_DRAW;

		// fill vertexbuffer
		size_t numChars = endChar - startChar;
		std::unique_ptr<CharacterVertex[]> vertices(new CharacterVertex[numChars]);
		unsigned int vertexIndex = 0;
		for(; startChar<endChar; ++startChar)
		{
			char character = text[startChar];
			if(character == '\n')
			{
				screenPos.x = textPosition_pixel.x;
				screenPos.y += fontSheet.GetCharHeight() + 5;
				continue;
			}
			else if(character == ' ' || !fontSheet.ContainsChar(character))
			{
				screenPos.x += fontSheet.GetSpaceWidth();
				continue;
			}
			auto rect = fontSheet.GetCharRect(character);

			vertices[vertexIndex].uperLeft_ScreenPix = screenPos;
			vertices[vertexIndex].width_ScreenPix = static_cast<float>(rect.right - rect.left);
			vertices[vertexIndex].uperLeft_FontAtlasPix.x = static_cast<float>(rect.left);
			vertices[vertexIndex].uperLeft_FontAtlasPix.y = static_cast<float>(rect.top);

			screenPos.x += vertices[vertexIndex].width_ScreenPix + charMargin;
			++vertexIndex;
		}
		_fontVertexBuffer->Write(vertices.get(), sizeof(CharacterVertex) * vertexIndex);

		// draw
		DeviceManager::Get().GetContext()->Draw(vertexIndex, 0);
	}

	// reset states
	DeviceManager::Get().SetRasterizerState(RasterizerState::CullFront);
	DeviceManager::Get().SetBlendState(BlendState::Opaque);
}