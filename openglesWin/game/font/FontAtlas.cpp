#include "PreCompile.h"
#include "FontAtlas.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include "FTBitmapChar.h"
#include "TreeNode.h"
#include <algorithm>
#include "ResManager.h"
#include "FTBitmapFont.h"
#ifndef WIN32
#include <android/log.h>
#endif

FontAtlas::FontAtlas()
{
	m_useKerning = false;
	m_pLibrary = NULL;
	m_pShowAtlas = NULL;
}

FontAtlas::~FontAtlas()
{
	int n;
	for (n = 0; n < (int)m_listBitmapChar.size(); n++) 
	{
		delete m_listBitmapChar[n];
	}
	for (n = 0; n < (int)m_listBitmapFont.size(); n++) 
	{
		delete m_listBitmapFont[n];
	}
	if (m_pShowAtlas) delete m_pShowAtlas;
	ReleaseTexture();
	ReleaseLibrary();
}

void FontAtlas::ReleaseLibrary()
{
	if (m_pLibrary)
	{
		FT_Done_FreeType(m_pLibrary);
		m_pLibrary = NULL;
	}
}


void FontAtlas::AddFont(unsigned char data[], unsigned int font_size, int size, const char* szLetters)
{
	if (m_pLibrary == NULL)
	{
		FT_Init_FreeType( &m_pLibrary );
			//throw RacException("FT_Init_FreeType failed");
	}

	// The Object In Which FreeType Holds Information On A Given
	// Font Is Called A "face".
	FT_Face face;

	// This Is Where We Load In The Font Information From The File.
	FT_New_Memory_Face(m_pLibrary, (FT_Byte *) data, font_size, 0, &face );

	// FreeType Measures Font Size In Terms Of 1/64ths Of Pixels.  
	FT_Set_Char_Size(face, size*64, size*64, 72, 72);

	int len = strlen(szLetters);
	int n;
	FTBitmapChar* pFTBitmapChar;
	FT_Glyph pGlyph;
	FTBitmapFont* pFTBitmapFont = new FTBitmapFont(this);
	pFTBitmapFont->SetLineHeight(face->size->metrics.height>>6);
	pFTBitmapFont->SetFTFace(face);
	m_listBitmapFont.push_back(pFTBitmapFont);
	unsigned char c;
	int height;
	int yOffset;
	int ixGlyph;
	for (n = 0; n < len; n++)
	{
		c = szLetters[n];
		// chaeck that the character hasn't already been processed
		if (pFTBitmapFont->GetChar(c) == NULL)
		{
			// Load The Glyph For Our Character.
			ixGlyph = FT_Get_Char_Index(face, c);
			if (ixGlyph == 0)
				cout << "character doesn't exist in font: " << c << endl;
			else
			{
				FT_Load_Glyph(face, ixGlyph, FT_LOAD_DEFAULT);
					//throw RacException("FT_Load_Glyph failed");

				// Move The Face's Glyph Into A Glyph Object.
				FT_Get_Glyph(face->glyph, &pGlyph );
					//throw RacException("FT_Get_Glyph failed");

				pFTBitmapChar = new FTBitmapChar(c,this);
				// all metrics dimensions are multiplied by 64, so we have to divide by 64
				height = face->glyph->metrics.height >> 6;
				yOffset = pFTBitmapFont->GetLineHeight()-(face->glyph->metrics.horiBearingY >> 6) ;
				pFTBitmapChar->SetOffsets(face->glyph->metrics.horiBearingX >> 6, 
					yOffset);
				pFTBitmapChar->SetSize(face->glyph->metrics.width >> 6, height);
				pFTBitmapChar->SetXAdvance(face->glyph->metrics.horiAdvance >> 6);
				pFTBitmapChar->SetGlyph(pGlyph);
				m_listBitmapChar.push_back(pFTBitmapChar);
				pFTBitmapFont->AddChar(c, pFTBitmapChar);
			}
		}
	}
}

// gets next biggest texture size in follwing sequence: 32x32, 64x32, 64x64, 128x64, etc.
void GetNextTextureSize(int &texWidth, int &texHeight, int ixSize)
{
	if (ixSize % 2)
		texHeight *= 2;
	else
		texWidth *= 2;
	if (texWidth > 1024 || texHeight > 1024)
	{
		printf("to many images to fit in one texture");
		//throw RacException("to many images to fit in one texture");
	}
}

// used by std::sort
bool GreaterSize(FTBitmapChar* pFTBitmapChar1, FTBitmapChar* pFTBitmapChar2) 
{ 
	return pFTBitmapChar1->GetNumPixels() > pFTBitmapChar2->GetNumPixels(); 
}

void FontAtlas::CreateAtlas()
{
	int n;
	int totalPixels = 0;
	for (n = 0; n < (int)m_listBitmapChar.size(); n++) 
	{
		totalPixels += m_listBitmapChar[n]->GetNumPixels();
	}

	int ixSize = 0;
	int texWidth = 32;
	int texHeight = 32;
	while (true) 
	{
		if (totalPixels <= texWidth*texHeight)
		{
			break;
		}
		GetNextTextureSize(texWidth, texHeight, ixSize);
		ixSize++;
	}
	sort(m_listBitmapChar.begin(), m_listBitmapChar.end(), GreaterSize); 
	TreeNode::GetAllocator().Init((int)(m_listBitmapChar.size()+1)*2);
	while (!BinPack(texWidth, texHeight)) 
	{
		TreeNode::GetAllocator().Deallocate();
		GetNextTextureSize(texWidth, texHeight, ixSize);
		ixSize++;
	}
	TreeNode::GetAllocator().Release();
	unsigned char* pData = new unsigned char[texWidth * texHeight];
#ifdef _DEBUG
	memset(pData, 0, texWidth * texHeight);		// only need this to view atlas
#endif
	for (n = 0; n < (int)m_listBitmapChar.size(); n++) 
	{
		m_listBitmapChar[n]->DrawToBitmap(pData, texWidth, texHeight);
		m_listBitmapChar[n]->ReleaseGlyph();
	}

	for (n = 0; n < (int)m_listBitmapFont.size(); n++) 
	{
		m_listBitmapFont[n]->FinishCreating();
	}
	if (!m_useKerning)
		ReleaseLibrary();

	glEnable(GL_TEXTURE_2D); 
	glGenTextures(1, &m_textureID);	
	glBindTexture( GL_TEXTURE_2D, m_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, texWidth, texHeight,
		0, GL_ALPHA, GL_UNSIGNED_BYTE, pData);
	int err = glGetError();
	if (err != GL_NO_ERROR)
	{
		//throw RacException("Error in glTexImage2D: ", err);
	}
	// clean up memory
	delete [] pData;
	m_pShowAtlas = new FTBitmapChar();
	m_pShowAtlas->SetXY(0, 0);
	m_pShowAtlas->SetSize(texWidth, texHeight);
	m_pShowAtlas->InitTexCoords(texWidth, texHeight);
}

void FontAtlas::RenderAtlas(int x, int y)
{
	//g_resManager.GetBatcher().SetAttribs(m_textureID, 0, 1.0f, true, false);
	m_pShowAtlas->Render(x, y, 0xffffff);
}


bool FontAtlas::BinPack(int texWidth, int texHeight)
{
	TreeNode* pTreeNode = TreeNode::GetAllocator().New();
	pTreeNode->Set(0, 0, texWidth, texHeight); 
	for (int n = 0; n < (int)m_listBitmapChar.size(); n++) 
	{
		if (!pTreeNode->Add(m_listBitmapChar[n])) 
		{
			return false;
		}
	} 
	return true;
}


void FontAtlas::ReleaseTexture()
{
	// note: make sure you do this before shutting down opengl
	if (m_textureID != 0)
	{
		glDeleteTextures(1, &m_textureID); 
		m_textureID = 0;
	}
}
