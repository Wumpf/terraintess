#include "stdafx.h"
#include "FontSheet.h"
#include "Texture.h"
#include "DeviceManager.h"

#include <Gdiplus.h>
#include <cassert>
  
FontSheet::FontSheet(const std::wstring& fontName, float pixelFontSize, FontStyle fontStyle, bool antiAliased) :
	_fontSheetTexture(nullptr),
	_texWidth(1024),
	_texHeight(0), // to be calculated
	_spaceWidth(0),
	_charHeight(0)
{
	// Init GDI+
	ULONG_PTR token = NULL;
	Gdiplus::GdiplusStartupInput startupInput(NULL, TRUE, TRUE);
	Gdiplus::GdiplusStartupOutput startupOutput;
	Gdiplus::GdiplusStartup(&token, &startupInput, &startupOutput);

	// Create a new scope so all GDI+ objects are destroyed before we shutdown GDI+.
	{
		Gdiplus::Font font(fontName.c_str(), pixelFontSize, (int)fontStyle, Gdiplus::UnitPixel, nullptr);

		// The bitmap of antialiased text might look "blocky", but you have to look at the alpha channel which has the smooth edges.  
		Gdiplus::TextRenderingHint hint = antiAliased ? Gdiplus::TextRenderingHintAntiAlias : Gdiplus::TextRenderingHintSystemDefault;

		// Bitmap for drawing a single char.
		int tempSize = static_cast<int>(pixelFontSize * 2);
		Gdiplus::Bitmap charBitmap(tempSize, tempSize, PixelFormat32bppARGB);
		Gdiplus::Graphics charGraphics(&charBitmap);
		charGraphics.SetPageUnit(Gdiplus::UnitPixel);
		charGraphics.SetTextRenderingHint(hint);
		
		// Determine _charHeight, _spaceWidth, and _texHeight.
		MeasureChars(font, charGraphics);

		// Bitmap for storing all the char sprites on a sprite sheet.
		Gdiplus::Bitmap fontSheetBitmap(_texWidth, _texHeight, PixelFormat32bppARGB);
		Gdiplus::Graphics fontSheetGraphics(&fontSheetBitmap);
		fontSheetGraphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
		fontSheetGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));

		BuildFontSheetBitmap(font, charGraphics, charBitmap, fontSheetGraphics);

//    Uncomment for debugging to save font sheet as .bmp file.
	/*	CLSID clsid;
		GetEncoderClsid(L"image/bmp", &clsid);
		fontSheetBitmap.Save(L"font.bmp", &clsid, NULL);
		*/
		BuildFontSheetTexture(fontSheetBitmap);
		assert(_fontSheetTexture.get() && "font creation failed");
	}

	// Shutdown GDI+: You must delete all of your GDI+ objects 
	// (or have them go out of scope) before you call GdiplusShutdown.
	Gdiplus::GdiplusShutdown(token);
}

FontSheet::~FontSheet()
{
}

void FontSheet::MeasureChars(Gdiplus::Font& font, Gdiplus::Graphics& charGraphics)
{
	WCHAR allChars[_nu_chars + 1];
	for(WCHAR i = 0; i < _nu_chars; ++i)
		allChars[i] = _startChar + i;
	allChars[_nu_chars] = 0;

	Gdiplus::RectF sizeRect;
	charGraphics.MeasureString(allChars, _nu_chars, &font, Gdiplus::PointF(0, 0), &sizeRect);
	_charHeight = static_cast<int>(sizeRect.Height + 0.5f);  

	// Given the fixed texture width, figure out how many rows we need to draw
	// all the characters, and consequently how much texture height we need.
	int numRows = static_cast<int>(sizeRect.Width / _texWidth) + 1;  
	_texHeight  = static_cast<int>(numRows*_charHeight) + 1;

	// Measure space character (which we do not store in the atlas).
	WCHAR charString[2] = {' ', 0};
	charGraphics.MeasureString(charString, 1, &font, Gdiplus::PointF(0, 0), &sizeRect);
	_spaceWidth = static_cast<int>(sizeRect.Width + 0.5f);  
}

void FontSheet::BuildFontSheetBitmap(Gdiplus::Font& font, Gdiplus::Graphics& charGraphics, Gdiplus::Bitmap& charBitmap, Gdiplus::Graphics& fontSheetGraphics)
{
	WCHAR charString[2] = {' ', 0};
	Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));
	UINT fontSheetX = 0;
	UINT fontSheetY = 0;
	for(UINT i = 0; i < _nu_chars; ++i)
	{
		charString[0] = static_cast<WCHAR>(_startChar + i);
		charGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));
		charGraphics.DrawString(charString, 1, &font, Gdiplus::PointF(0.0f, 0.0f), &whiteBrush);

		// Compute tight char horizontal bounds (ignoring empty space).
		int minX = GetCharMinX(charBitmap);
		int maxX = GetCharMaxX(charBitmap);
		int charWidth = maxX - minX + 1;

		// Move to next row of the font sheet?
		if(fontSheetX + charWidth >= _texWidth)
		{
			fontSheetX = 0;
			fontSheetY += static_cast<int>(_charHeight) + 1;
		}

		// Save the rectangle of this character on the texture atlas.
		_charRects[i] = CD3D11_RECT(fontSheetX, fontSheetY, fontSheetX + charWidth, fontSheetY + _charHeight);

		// The rectangle subset of the source image to copy.
		fontSheetGraphics.DrawImage(&charBitmap, fontSheetX, fontSheetY, 
			minX, 0, charWidth, _charHeight, Gdiplus::UnitPixel);

		// next char
		fontSheetX += charWidth + 1;
	}
}


void FontSheet::BuildFontSheetTexture(Gdiplus::Bitmap& fontSheetBitmap)
{
	// Lock the bitmap for direct memory access
	Gdiplus::BitmapData bmData;
	fontSheetBitmap.LockBits(&Gdiplus::Rect(0, 0, _texWidth, _texHeight), Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData);  
	_fontSheetTexture =  Texture2D::CreateFromData(bmData.Scan0, DXGI_FORMAT_B8G8R8A8_UNORM, _texWidth, _texHeight, 
										Texture2D::CreationFlags::AUTOGEN_MIPMAPS | Texture2D::CreationFlags::SHADERRES_VIEW);
	fontSheetBitmap.UnlockBits(&bmData);  
}

int FontSheet::GetCharMinX(Gdiplus::Bitmap& charBitmap)
{
	int width  = charBitmap.GetWidth();
	int height = charBitmap.GetHeight();
	
	for(int x = 0; x < width; ++x)
	{
		for(int y = 0; y < height; ++y)
		{
			Gdiplus::Color color;
			charBitmap.GetPixel(x, y, &color);
			if(color.GetAlpha() > 0)
			{
				 return x;
			}
		}
	}

	return 0;
}

int FontSheet::GetCharMaxX(Gdiplus::Bitmap& charBitmap)
{
	int width  = charBitmap.GetWidth();
	int height = charBitmap.GetHeight();

	for(int x = width-1; x >= 0; --x)
	{
		for(int y = 0; y < height; ++y)
		{
			Gdiplus::Color color;
			charBitmap.GetPixel(x, y, &color);
			if(color.GetAlpha() > 0)
			{
				 return x;
			}
		}
	}
	return width-1;
}

int FontSheet::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);

	return -1;  // Failure
}