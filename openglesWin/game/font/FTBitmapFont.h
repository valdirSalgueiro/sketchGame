#ifndef FTBITMAPFONT_H_
#define FTBITMAPFONT_H_

class FTBitmapChar;
class FontAtlas;

using namespace std;

class FTBitmapFont
{
public:
	FTBitmapFont(FontAtlas* pFontAtlas);
	~FTBitmapFont();
	void Load(const string& fileName);
	void DrawVertical(int x, int y, const string& text, int color, float alpha = 1.0f);
	// draws the string at position x, y (given in screen coordinates)
	int DrawString(int x, int y, const string& text, int color, float alpha = 1.0f) 
	{
		return DrawString(x, y, text.c_str(), color, alpha);
	}	
	int DrawStringShadow(int x, int y, const string& text, int color, int backColor) 
	{
		return DrawStringShadow(x, y, text.c_str(), color, backColor);
	}	
	int DrawString(int x, int y, const char* text, int color, float alpha = 1.0f);
	int DrawStringShadow(int x, int y, const char* text, int color, int backColor);
	int GetWidth(const char* text);
	int GetLineHeight() const
	{
		return m_lineHeight;
	}
	void SetLineHeight(int lineHeight)
	{
		m_lineHeight = lineHeight;
	}
	void Release();
	void Init();
	int GetCharWidth(const string& text, int index);
	void AddChar(int charCode, FTBitmapChar* pFTBitmapChar)
	{
		m_mapBitmapChar[charCode] = pFTBitmapChar;
	}
	void SetFTFace(struct FT_FaceRec_* pFTFace)
	{
		m_pFTFace = pFTFace;
	}
	void FinishCreating();
	FTBitmapChar* GetChar(int charCode)
	{
		fmap<int, FTBitmapChar*>::const_iterator iter;
		iter = m_mapBitmapChar.find(charCode);
		if (iter != m_mapBitmapChar.end()) 
			return iter->second;
		return NULL;
	}
private:
	void ReleaseFace();
	int m_lineHeight;
	fmap<int, FTBitmapChar*> m_mapBitmapChar;
	FontAtlas* m_pFontAtlas;
	struct FT_FaceRec_* m_pFTFace;
};

#endif /*FTBITMAPFONT_H_*/
