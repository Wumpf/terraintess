#pragma once

template<class T> class ConstantBuffer;

class FontRenderer
{
public:
	FontRenderer();
	~FontRenderer();

	/// draws a string
	/// \param marginFactor		margin between single chars is expressed as SpaceWidth * marginFactor
	void DrawString(class FontSheet& fontSheet, const std::string text, const SimpleMath::Vector2& textPosition_pixel, const SimpleMath::Color& color = SimpleMath::Color(1.0f, 1.0f, 1.0f),
					float marginFactor = 0.25f);

private:
	struct CharacterVertex
	{
		SimpleMath::Vector2 uperLeft_ScreenPix;
		float width_ScreenPix;
		SimpleMath::Vector2 uperLeft_FontAtlasPix;

		static const D3D11_INPUT_ELEMENT_DESC desc[];
		static const unsigned int numDescElements;
	};

	std::unique_ptr<class BufferObject> _fontVertexBuffer;
	std::unique_ptr<class Effect> _fontShaders;

	struct FontSettings
	{
		float Height_clipSpace;
		float Height_texSpace;
		SimpleMath::Vector2 FontAtlasSize;
		SimpleMath::Color Color;
	};
	std::unique_ptr<ConstantBuffer<FontSettings>> _constantBuffer;

	static const unsigned int MAX_CHARS_PER_DRAW = 1024;
};

