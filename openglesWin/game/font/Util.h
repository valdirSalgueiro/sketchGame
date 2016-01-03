#ifndef _UTIL_H_INCLUDED_
#define _UTIL_H_INCLUDED_

#ifdef _WIN32
const char FILE_SEPERATOR = '\\';
#else
const char FILE_SEPERATOR = '/';
#endif

struct WndStruct
{
	int dummy;
};


extern WndStruct* g_hWnd;

int GetMilliSeconds();




#endif // _UTIL_H_INCLUDED_
