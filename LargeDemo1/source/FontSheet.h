#pragma once

// --------------------------------------------------
// based on http://www.d3dcoder.net/resources.htm
// --------------------------------------------------

namespace Gdiplus
{
	class Bitmap;
	class Font;
	class Graphics;
}

/// A font sheet is a texture atlas that stores characters for the given font
/// to be used with the SpriteBatch class for rendering text. 
///
/// There is a fair amount of cost to create the font sheet, as we use GDI+ to 
/// render out each character, and then copy the sheet to a texture.  Therefore, 
/// all the FontSheets an application needs should be created at initialization
/// time.
class FontSheet
{
public:

	// Mirrors the Gdiplus FontStyle enum.
	enum class FontStyle 
	{
		Regular      = 0,
		Bold         = 1,
		Italic       = 2,
		BoldItalic   = 3,
		Underline    = 4,
		Strikeout    = 8 
	};

public:

	FontSheet(const std::wstring& fontName, float pixelFontSize, FontStyle fontStyle, bool antiAliased = true);
	~FontSheet();

	/// Gets the font sheet texture atlas.
	std::shared_ptr<class Texture2D> GetFontTexture()			{ return _fontSheetTexture; }

	int GetFontTextureHeight() const { return _texHeight; }
	int GetFontTextureWidth() const { return _texWidth; }

	/// Gets the rectangle on the sprite sheet that bounds the given character.
	const CD3D11_RECT& GetCharRect(wchar_t c) const		{ assert(ContainsChar(c)); return _charRects[c - _startChar]; }

	/// Gets the width of the "space" character.
    int GetSpaceWidth() const { return _spaceWidth; }
	/// Returns the character height for the font. This should be used for newline characters when rendering text.
    int GetCharHeight() const { return _charHeight; }

	bool ContainsChar(wchar_t c) const { return c >= _startChar && c < _endChar; }


private:
	FontSheet(const FontSheet& rhs);
	FontSheet& operator=(const FontSheet& rhs);

	/// Determines _charHeight, _spaceWidth, and _texHeight.
	void MeasureChars(Gdiplus::Font& font, Gdiplus::Graphics& charGraphics);

	/// Draw the characters one-by-one to the charBitmap, and then copy to the sprite sheet.
	void BuildFontSheetBitmap(Gdiplus::Font& font, Gdiplus::Graphics& charGraphics, Gdiplus::Bitmap& charBitmap, Gdiplus::Graphics& fontSheetGraphics);
	/// Copies the GDI+ font sheet to a d3d11 texture.
	void BuildFontSheetTexture(Gdiplus::Bitmap& fontSheetBitmap);

	/// Scans column-by-column to look for the left-most opaque pixel. We cannot scan row-by-row, as the first opaque pixel we find might not be "left-most".
	int GetCharMinX(Gdiplus::Bitmap& charBitmap);
	/// Scans column-by-column to look for the right-most opaque pixel. We cannot scan row-by-row, as the first opaque pixel we find might not be "right-most".
	int GetCharMaxX(Gdiplus::Bitmap& charBitmap);

	/// For saving the GDI Bitmap to file (for internal debugging).
	int GetEncoderClsid(const wchar_t* format, CLSID* pClsid);

private:
	// ASCII characters from 33='!' to 127.  
	static const wchar_t _startChar = 33;
	static const wchar_t _endChar = 127;
	static const unsigned int _nu_chars = _endChar - _startChar;

	std::shared_ptr<Texture2D> _fontSheetTexture;

	unsigned int _texWidth;
	unsigned int _texHeight;

	CD3D11_RECT _charRects[_nu_chars];
	int _spaceWidth;
	int _charHeight;  
};
