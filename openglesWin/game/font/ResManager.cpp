#include "PreCompile.h"
#include "ResManager.h"
#include "Util.h"
#include "FontAtlas.h"

ResManager g_resManager;

ResManager::ResManager() : m_initialised(false)
{
}

ResManager::~ResManager()
{
}

void ResManager::Release()
{
	m_batcher.Release();
	delete m_pFontAtlas;
}

void ResManager::Init(const string& dirResources, const string& dirUser, 
	int screenWidth, int screenHeight)
{
	m_dirUser = dirUser + FILE_SEPERATOR;
	m_dirResources = dirResources + FILE_SEPERATOR;
#ifdef _DEBUG
	cout << m_dirResources << endl;
	cout << m_dirUser << endl;
#endif
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DITHER);	// these are enabled by default
	glDisable(GL_ALPHA_TEST);
	
	glViewport(0, 0, m_screenWidth, m_screenHeight);
	m_initialised = true;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glFrontFace(GL_CW);
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// set orthographic, so that we are drawing billboard style using screen dimensions 
	glOrthof(0, (float)m_screenWidth, (float)m_screenHeight, 0, -50.0f, 50.0f);

	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_DEPTH_TEST);	
	// handle tranparancy
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const char* szLetters = " !\"#&'()*,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\_abcdefghijklmnopqrstuvwxyzÁáÉéÍíÑñÓóÚú";

	m_pFontAtlas = new FontAtlas();
	//m_pFontAtlas->AddFont("arial.ttf", 12, szLetters);
	//m_pFontAtlas->AddFont("times.ttf", 18, szLetters);
	//m_pFontAtlas->AddFont("times.ttf", 16, szLetters);
	m_pFontAtlas->CreateAtlas();


}

FTBitmapFont* ResManager::GetFTFont(int index) 
{
	return m_pFontAtlas->GetFont(index);
}
