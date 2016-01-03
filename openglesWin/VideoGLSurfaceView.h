#include <string.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
//#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <time.h>

//#pragma comment(lib, "avcodec.lib")
//#pragma comment(lib, "avformat.lib")
//#pragma comment(lib, "avutil.lib")
//#pragma comment(lib, "swscale.lib")

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
//#define HIGHRES

#ifdef WIN32
	#ifdef HIGHRES
		#define FRAME_X 3200
		#define FRAME_Y 1920
	#else
		#define FRAME_X 320
		#define FRAME_Y 192
	#endif
#else
	#define FRAME_X 320
	#define FRAME_Y 192
#endif

#define FPS_TRIGGER 2

bool native_gl_render(float elapse);

void printFramesPerSecond();

bool grabFrame(float elapsed);
void native_init(const char * path);
void destroy_();
