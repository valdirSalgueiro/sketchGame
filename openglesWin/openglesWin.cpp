//#include "vld.h"
#include <windows.h>

#include "GLES/gl.h"
#include "GLES/egl.h"

#include <stdio.h>
#include <string>

#include "game\gl2d.h"
#include "game\Engine.h"

#include <cmath>

#include <SDL.h>
#include "game\gmUtil.h"

using namespace std;

#define POWERVR

//#include "Game.h"

//static Game game;

static void CreateConsole(void);
static void DestroyConsole(void);

static LONG WINAPI	MainWndProc(HWND, UINT, WPARAM, LPARAM);


Engine engine;

bool isPressedRot=false;
bool isPressedPos=false;

int rotActionID=-1;
int posActionID=-1;

#define M_PI 3.14159
// The radius of the touchable region.
#define JOYSTICK_RADIUS 64.0f
//#define JOYSTICK_RADIUS 92.0f

// How far from the center should the thumb be allowed to move?
// Used only for visual feedback not for velocity calculations.
#define THUMB_RADIUS 48.0f
//#define THUMB_RADIUS 72.0f

int dir=0;

bool isPointInCircle(Vector2D<float> point,Vector2D<float> center, float radius){
	float dx = (point.x - center.x);
	float dy = (point.y - center.y);
	return (radius*radius >= (dx * dx) + (dy * dy));
}

static double round(double num)
{
	double integer = ceil(num);
	if (num > 0)
		return integer - num > 0.5 ? integer - 1.0 : integer;
	return integer - num >= 0.5 ? integer - 1.0 : integer;
}
static float roundf(float num)
{
	float integer = ceilf(num);
	if (num > 0)
		return integer - num > 0.5f ? integer - 1.0f : integer;
	return integer - num >= 0.5f ? integer - 1.0f : integer;
}

SDL_Window* m_window;

extern bool touch;

void processInput(int x, int y){
	//engine.isShooting=isPressedRot;

	if(isPointInCircle(Vector2D<float>(x,y),Vector2D<float>(engine.controlBasePos),JOYSTICK_RADIUS)){
		isPressedPos=true;
	}else{
		touch=true;
	}

	if(isPressedPos){
		//DPAD

		float dx = x - engine.controlBasePos.x;
		float dy = y - engine.controlBasePos.y;
		float angle = atan2(dy, dx); // in radians

		float anglePerSector = 360.0f / 4 * (M_PI/180.0f);
		angle = roundf(angle/anglePerSector) * anglePerSector;

		Vector2D<float> velocity = Vector2D<float>(cos(angle), sin(angle));

		engine.controlKnobPos.x = engine.controlBasePos.x + cos(angle) * THUMB_RADIUS;
		engine.controlKnobPos.y = engine.controlBasePos.y + sin(angle) * THUMB_RADIUS;

		//__android_log_print(ANDROID_LOG_INFO, "adMOB", "%f %f", velocity.x, velocity.y);

		dir=0;
		if(((int) velocity.x)==-1){
			dir|=LEFT;
		}
		else if(((int) velocity.y)==1){
			dir|=DIRECTION::DOWN;
		}
		else if(((int) velocity.x)==1){
			dir|=DIRECTION::RIGHT;
		}
		else if(((int) velocity.y)==-1){
			dir|=DIRECTION::UP;
		}
	}
}


void OnEvent(SDL_Event* Event){
	switch(Event->type) {
	case SDL_KEYDOWN: 
		switch( Event->key.keysym.sym )
		{
		case SDLK_RIGHT:
			dir|=DIRECTION::RIGHT;
			break;
		case SDLK_LEFT:
			dir|=DIRECTION::LEFT;
			break;
		case SDLK_UP: 
			dir|=DIRECTION::UP;

			break;
		case SDLK_DOWN: 
			dir|=DIRECTION::DOWN;
			break;

		case SDLK_z:
			touch=true;
			break;
		}
		
		break;


	case SDL_KEYUP:
				switch( Event->key.keysym.sym )
		{
		case SDLK_RIGHT:
			dir&=~DIRECTION::RIGHT;
			break;
		case SDLK_LEFT:
			dir&=~DIRECTION::LEFT;
			break;
		case SDLK_UP: 
			dir&=~DIRECTION::UP;
			break;
		case SDLK_DOWN: 
			dir&=~DIRECTION::DOWN;
			break;
		case SDLK_z:
			touch=false;
			break;
		}

		break;


	case SDL_MOUSEMOTION:
		if(Event->motion.state&SDL_BUTTON(SDL_BUTTON_LEFT)){
			processInput(Event->motion.x,Event->motion.y);
		}
		break;


	case SDL_MOUSEBUTTONDOWN:
		switch(Event->button.button){
		case SDL_BUTTON_LEFT:
			processInput(Event->button.x,Event->button.y);
			break;

		case SDL_BUTTON_RIGHT: 
			break;

		case SDL_BUTTON_MIDDLE:
			break;

		}
		break;



	case SDL_MOUSEBUTTONUP:   
		switch(Event->button.button) {
		case SDL_BUTTON_LEFT: 
			isPressedPos=false;
			engine.controlKnobPos=engine.controlBasePos;
			dir=0;
			touch=false;
			break;									  

		case SDL_BUTTON_RIGHT: 
			break;									   

		case SDL_BUTTON_MIDDLE: 
			break;

		}
		break;
	}
}

float wcrpg_time_step = 0.0f; /* Current system time step */
float elapsed = 0.0f; /* Current system time step */
LARGE_INTEGER wcrpg_time_measure; /* Measured time */
LARGE_INTEGER wcrpg_time_final; /* Time ended */
LARGE_INTEGER wcrpg_time_freq; /* Frequency of cycles */
int wcrpg_fps = 0; /* Frames per second */

void time_start(){
	QueryPerformanceCounter(&wcrpg_time_measure);
}

void time_end(){
	/* Find delta */
	QueryPerformanceCounter(&wcrpg_time_final);
	wcrpg_time_final.QuadPart -= wcrpg_time_measure.QuadPart;
	QueryPerformanceFrequency(&wcrpg_time_freq);
	wcrpg_time_step = (float)(wcrpg_time_final.QuadPart)/(float)(wcrpg_time_freq.QuadPart);
	elapsed+=wcrpg_time_step;
}


void altInit(int width, int height){
	startBatch(width,height);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CoInitialize(NULL);

	MSG 			msg; 
	WNDCLASS		wndclass;
	HWND			hWnd;
	HDC				hDC;

	const EGLint s_configAttribs[] =
	{
		EGL_LEVEL,				0,
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
		EGL_NATIVE_RENDERABLE,	EGL_FALSE,
		EGL_DEPTH_SIZE,			EGL_DONT_CARE,
		EGL_SAMPLE_BUFFERS, 1 /* true */,
        EGL_SAMPLES, 8,
		EGL_NONE
	};

	EGLDisplay	eglDisplay;
	EGLConfig	eglConfig;
	EGLContext	eglContext;
	EGLSurface	eglWindowSurface;
	EGLint numConfigs;
	EGLint majorVersion;
	EGLint minorVersion;

	BOOL			run;

	wndclass.style		   = 0;
	wndclass.lpfnWndProc   = (WNDPROC)MainWndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance	   = hInstance;
	wndclass.hIcon		   = 0;
	wndclass.hCursor	   = LoadCursor (NULL,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wndclass.lpszMenuName  = 0;
	wndclass.lpszClassName = "Green Juice Team";


	if (!RegisterClass(&wndclass))
	{
		return FALSE;
	}


	hWnd = CreateWindow( "Green Juice Team",
		"Green Juice Team",
		WS_VISIBLE | WS_POPUP |	WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		//1920,
		//1080,
		800,
		480,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hWnd)
	{
		return FALSE;
	}	

	//SetWindowPos(hWnd, 0, (GetSystemMetrics(SM_CXSCREEN)/2)-400, (GetSystemMetrics(SM_CYSCREEN)/2)-240, 0, 0, SWP_NOSIZE | SWP_NOZORDER);


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	SDL_Init(SDL_INIT_VIDEO);
	m_window = SDL_CreateWindowFrom(hWnd);

#ifdef _DEBUG
	CreateConsole();
#endif

#ifdef POWERVR
	hDC = GetDC(hWnd);

	eglDisplay = eglGetDisplay(hDC);
#else
	hDC = 0;

	eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif
	eglInitialize(eglDisplay, &majorVersion, &minorVersion);
	eglGetConfigs(eglDisplay, NULL, 0, &numConfigs);
	eglChooseConfig(eglDisplay, s_configAttribs, &eglConfig, 1, &numConfigs);

	EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, ai32ContextAttribs);

	eglWindowSurface = eglCreateWindowSurface(eglDisplay, eglConfig, hWnd, NULL);
	eglMakeCurrent(eglDisplay, eglWindowSurface, eglWindowSurface, eglContext);	

	run = TRUE;

	SDL_Event Event;
	int frames=0;

	while (run)
	{
		time_start();

		static struct tagRECT lastrc = {-1, -1, -1, -1};	 
		static struct tagRECT rc;

		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
		{
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				run = FALSE;
			}
		}

		while(SDL_PollEvent(&Event)) {
			 OnEvent(&Event);
		}

		GetClientRect(hWnd, &rc);

		if (rc.right != lastrc.right || rc.bottom != lastrc.bottom)
		{
			altInit(rc.right,rc.bottom);
			engine.init(rc.right,rc.bottom);

			lastrc = rc;
		}	

		glClearScreen();
		

		engine.handleInput(1,dir);

		if(elapsed>=1.0f/60.0f){
			engine.update(elapsed);
			elapsed=0;		
		}
		engine.render(wcrpg_time_step);

		eglSwapBuffers(eglDisplay, eglWindowSurface);

		time_end();
	}
	glFree();

	eglMakeCurrent(eglDisplay, NULL, NULL, NULL);
	eglDestroyContext(eglDisplay, eglContext);
	eglDestroySurface(eglDisplay, eglWindowSurface);
	eglTerminate(eglDisplay);

#ifdef _DEBUG
	DestroyConsole();
#endif

	if (hDC)
	{
		ReleaseDC(hWnd, hDC);
	}

	DestroyWindow (hWnd);

	return 0;
}

static LONG WINAPI MainWndProc(HWND	hWnd, UINT uMsg, WPARAM	wParam, LPARAM lParam)
{
	LONG			lRet = 1;

	switch (uMsg)
	{
	case WM_CLOSE:
		{
			PostQuitMessage(0);
		}
		break;

	case WM_KEYDOWN:
		{
			switch ((int)wParam)
			{				
			case VK_ESCAPE:
				SendMessage(hWnd, WM_CLOSE, 0, 0);
			}
		}

	default:
		{
			lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		break;
	}

	return lRet;
}

static void CreateConsole(void)
{
	
	AllocConsole();
	freopen("CONIN$", "rb", stdin);
	freopen("CONOUT$", "wb", stdout);
	freopen("CONERR$", "wb", stderr);
	
	
}

static void DestroyConsole(void)
{
	
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
	FreeConsole();
	
	
}