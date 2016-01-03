#include "PreCompile.h"
#include "FTBitmapFont.h"
//#include "ResManager.h"
#include "FontAtlas.h"
#include "FTBitmapChar.h"
#include <ft2build.h>
#include <freetype/freetype.h>


FTBitmapFont::FTBitmapFont(FontAtlas* pFontAtlas) 
{
	m_pFontAtlas = pFontAtlas;
	m_pFTFace = NULL;
}


FTBitmapFont::~FTBitmapFont()
{
	ReleaseFace();
}

void FTBitmapFont::ReleaseFace()
{
	if (m_pFTFace) 
	{
		FT_Done_Face(m_pFTFace);
		m_pFTFace = NULL;
	}
}

int FTBitmapFont::GetWidth(const char* text) 
{
	int currX = 0;
	int n = 0;
	unsigned char c;
	fmap<int, FTBitmapChar*>::const_iterator iter;
	FT_Vector kerning;
	FT_UInt ixGlyph;
	FT_UInt ixGlyphPrev = 0;
	bool hasKerning = false;
	if (m_pFTFace)
	{
		hasKerning = FT_HAS_KERNING(m_pFTFace); 
	}
	while (text[n] != 0)
	{
		c = text[n];
		iter = m_mapBitmapChar.find(c);
		if (iter != m_mapBitmapChar.end()) 
		{
			if (hasKerning)
			{
				// get kerning
				ixGlyph = FT_Get_Char_Index(m_pFTFace, c); 
				if (hasKerning && ixGlyphPrev && ixGlyph) 
				{ 
					FT_Get_Kerning(m_pFTFace, ixGlyphPrev, ixGlyph, FT_KERNING_DEFAULT, &kerning); 
					currX += kerning.x >> 6; 
				} 
				ixGlyphPrev = ixGlyph;
			}
			currX += iter->second->GetXAdvance();
		}
		n++;
	} 
	return currX;
}	

int FTBitmapFont::GetCharWidth(const string& text, int index) 
{
	//if (index < (int)text.size())
	//	return m_hSpacing + m_chars[text[index]].GetXAdvance(&m_chars[text[index+1]]);
	//return m_hSpacing + m_chars[text[index]].GetXAdvance();
	return 0;
}	

int FTBitmapFont::DrawString(int x, int y, const char* text, int color, float alpha) 
{
	//g_resManager.GetBatcher().SetAttribs(m_pFontAtlas->GetTextureID(), color, alpha, true, false);
	unsigned char c;
	int currX = x;
	int n = 0;
	fmap<int, FTBitmapChar*>::const_iterator iter;
	FT_Vector kerning;
	FT_UInt ixGlyph;
	FT_UInt ixGlyphPrev = 0;
	bool hasKerning = false;
	if (m_pFTFace)
	{
		hasKerning = FT_HAS_KERNING(m_pFTFace); 
	}
	while (text[n] != 0)
	{
		c = text[n];
		iter = m_mapBitmapChar.find(c);
		if (iter != m_mapBitmapChar.end()) 
		{
			if (hasKerning)
			{
				// get kerning
				ixGlyph = FT_Get_Char_Index(m_pFTFace, c); 
				if (hasKerning && ixGlyphPrev && ixGlyph) 
				{ 
					FT_Get_Kerning(m_pFTFace, ixGlyphPrev, ixGlyph, FT_KERNING_DEFAULT, &kerning); 
					currX += kerning.x >> 6; 
				} 
				ixGlyphPrev = ixGlyph;
			}
			iter->second->Render(currX, y, color);
			currX += iter->second->GetXAdvance();
		}
		n++;
	} 
	return currX;
}	

int FTBitmapFont::DrawStringShadow(int x, int y, const char* text, int color, int backColor) 
{
#ifndef WIN32
	y=y+10;
#endif
	DrawString(x+2, y+2, text, backColor, 1.0f); 
	return DrawString(x, y, text, color, 1.0f); 
}	

void FTBitmapFont::FinishCreating()
{
	bool hasKerning = false;
	if (m_pFTFace)
	{
		hasKerning = FT_HAS_KERNING(m_pFTFace); 
	}
	if (!hasKerning || !m_pFontAtlas->GetUseKerning())
		ReleaseFace();
}


