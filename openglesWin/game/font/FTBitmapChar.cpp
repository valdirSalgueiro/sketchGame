#include "PreCompile.h"
#include "FTBitmapChar.h"
#include "GLCallBatcher.h"
//#include "ResManager.h"
#include <freetype/ftglyph.h>
#ifdef WIN32
#include "gl2d.h"
#else
#include "../gl2d.h"
#endif

FTBitmapChar::FTBitmapChar()
{
	m_x = 0;
	m_y = 0;
	m_width = 0;
	m_height = 0;
	m_xOffset = 0;
	m_yOffset = 0;
	m_xAdvance = 0;
	m_pGlyph = NULL;
}

FTBitmapChar::~FTBitmapChar()
{
}

	

void FTBitmapChar::InitTexCoords(int texWidth, int texHeight)
{
	float x1 = (float)m_x/(float)texWidth;
	float y1 = (float)m_y/(float)texHeight;
	float x2 = (float)(m_width)/(float)texWidth;
	float y2 = (float)(m_height)/(float)texHeight;

	//gambiarra
	m_texCoords[0] = x1;
	m_texCoords[1] = y1;
	m_texCoords[2] = x2;
	m_texCoords[3] = y2;
}

// setup quad and draw as triangle groups
void FTBitmapChar::Render(int x, int y, int color) const
{
	if (IsEmpty()) return;
	x += m_xOffset;	
	y += m_yOffset;
	float vertices[verticesPerQuad*compVertPos];

	const float inv256 = 1.0f/256.0f;
	float r = ((color>>16)&0xff)*inv256;
	float g = ((color>>8)&0xff)*inv256;
	float b = (color&0xff)*inv256;

	glImage image;
	image.width=m_width;
	image.height=m_height;
	image.textureID=fa->GetTextureID();

	image.u_off=m_texCoords[0];
	image.v_off=m_texCoords[1];
	image.u_width=m_texCoords[2];
	image.v_height=m_texCoords[3];

	
	spriteBatchDraw(x,y,GL2D_NO_SCALE | GL2D_FONT,&image,false,0,1,1,r,g,b,1);
} // end of draw()


void FTBitmapChar::DrawToBitmap(unsigned char* pData, int texWidth, int texHeight) 
{
	if (IsEmpty()) return;
	InitTexCoords(texWidth, texHeight);
	// Convert The Glyph To A Bitmap.
	FT_Glyph_To_Bitmap(&m_pGlyph, FT_RENDER_MODE_NORMAL, 0, 1);
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)m_pGlyph;

	// This Reference Will Make Accessing The Bitmap Easier.
	FT_Bitmap& bitmap = bitmap_glyph->bitmap;

	assert(bitmap.width == m_width);
	assert(bitmap.rows == m_height);
	int x, y = 0;
	int index;
	for (y = 0; y < bitmap.rows; y++)
	{
		for (x = 0; x < bitmap.width; x++)
		{
			index = (m_y+y)* texWidth + m_x + x;
			pData[index] = bitmap.buffer[y*bitmap.width + x];
		}
	}
}

void FTBitmapChar::ReleaseGlyph() 
{
	if (m_pGlyph) FT_Done_Glyph(m_pGlyph);
}