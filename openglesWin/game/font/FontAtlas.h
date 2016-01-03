#ifndef _FONTATLASMAKER_H_INCLUDED_
#define _FONTATLASMAKER_H_INCLUDED_


struct FT_LibraryRec_;
struct FT_FaceRec_;
class FTBitmapChar;
class FTBitmapFont;

using namespace std;

class FontAtlas
{
public:
	FontAtlas();
	~FontAtlas();
	void AddFont(unsigned char data[], unsigned int font_size, int size, const char* szLetters);
	void CreateAtlas();
	unsigned int GetTextureID() const
	{
		return m_textureID;
	}
	FTBitmapFont* GetFont(int index)
	{
		return m_listBitmapFont[index];
	}
	void RenderAtlas(int x, int y);
	bool GetUseKerning() const
	{
		return m_useKerning;
	}
	void SetUseKerning(bool useKerning)
	{
		m_useKerning = useKerning;
	}
private:
	bool BinPack(int width, int height);
	void ReleaseTexture();
	void ReleaseLibrary();
	struct FT_LibraryRec_* m_pLibrary;
	vector<FTBitmapChar*> m_listBitmapChar;
	unsigned int m_textureID;
	vector<FTBitmapFont*> m_listBitmapFont;
	vector<struct FT_FaceRec_*> m_listFace;
	FTBitmapChar* m_pShowAtlas;
	bool m_useKerning;
};





#endif // _FONTATLASMAKER_H_INCLUDED_
