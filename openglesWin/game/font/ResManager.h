#ifndef _RESMANAGER_H_INCLUDED_
#define _RESMANAGER_H_INCLUDED_

#include "GLCallBatcher.h"

class FontAtlas;
class FTBitmapFont;


class ResManager
{
public:
	ResManager();
	~ResManager();
	const string& GetDirResources() const
	{
		return m_dirResources;
	}
	int GetScreenHeight() const
	{
		return m_screenHeight;
	}
	int GetScreenWidth() const
	{
		return m_screenWidth;
	}
	bool IsInitalised() const
	{
		return m_initialised;
	}
	void Init(const string& dirResources, const string& dirUser, 
		int width, int height);
	void Release();
	GLCallBatcher& GetBatcher() 
	{
		return m_batcher;
	}
	const string& GetDirUser() const
	{
		return m_dirUser;
	}
	FTBitmapFont* GetFTFont(int index);
	FontAtlas* GetFontAtlas() const
	{
		return m_pFontAtlas;
	}
	string m_dirResources;
private:
	int m_screenWidth;
	int m_screenHeight;

	
	bool m_initialised;
	GLCallBatcher m_batcher;
	string m_dirUser;
	FontAtlas* m_pFontAtlas;
};

extern ResManager g_resManager;



#endif // _RESMANAGER_H_INCLUDED_
